#pragma once

#include "Math/myassert.h"
#include "Geometry/AABB2D.h"
#include "Utils/Pool.h"

#include <vector>
#include <list>
#include <utility>

template<typename T>
class Quadtree
{
public:
	// The elements in a node are linked
	class Element
	{
	public:
		T* object = nullptr;
		AABB2D aabb = {{0, 0}, {0, 0}};
		Element* next = nullptr;
	};

	// Nodes are as small as possible (8 bytes) to reduce memory usage and cache efficiency
	class QuadNode;
	class Node
	{
	public:
		void Add(Quadtree& tree, T* object, const AABB2D& object_aabb, unsigned depth, const AABB2D& node_aabb, bool optimizing)
		{
			if (IsBranch())
			{
				// Branch
				child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb, optimizing);
			}
			else if (depth == tree.max_depth || (unsigned) element_count < tree.max_node_elements)
			{
				// Leaf that can't split or leaf with space
				if (optimizing)
				{
					Element* new_first_element = tree.elements.Obtain();
					new_first_element->object = object;
					new_first_element->aabb = object_aabb;
					new_first_element->next = first_element;
					first_element = new_first_element;
				}
				else
				{
					Element element;
					element.object = object;
					element.aabb = object_aabb;
					size_t index = temp_element_list->size();
					temp_element_list->emplace_back(element);
					tree.num_added_elements += 1;
				}
				element_count += 1;
			}
			else
			{
				// Leaf with no space that can split
				Split(tree, depth, node_aabb, optimizing);
				child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb, optimizing);
			}
		}

		void Remove(Quadtree& tree, T* object)
		{
			if (IsBranch())
			{
				child_nodes->Remove(tree, object);
			}
			else
			{
				Element* element = first_element;
				Element** element_ptr = &first_element;
				while (element != nullptr)
				{
					if (element->object == object)
					{
						*element_ptr = element->next;
						tree.elements.Release(element);
						element_count -= 1;
					}

					element = element->next;
					element_ptr = &element->next;
				}
			}
		}

		void Split(Quadtree& tree, unsigned depth, const AABB2D& node_aabb, bool optimizing)
		{
			if (optimizing)
			{
				// Get first element before anything changes
				Element* element = first_element;

				// Transform leaf into branch
				element_count = -1;
				child_nodes = tree.quad_nodes.Obtain();
				child_nodes->Initialize();

				// Remove all elements and reinsert them
				while (element != nullptr)
				{
					T* object = element->object;
					AABB2D object_aabb = element->aabb;
					Element* next_element = element->next;
					tree.elements.Release(element);

					child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb, optimizing);

					element = next_element;
				}
			}
			else
			{
				// Get element vector before anything changes
				std::list<Element>* temp_elements = temp_element_list;

				// Transform leaf into branch
				element_count = -1;
				tree.aux_quad_nodes.emplace_back(QuadNode());
				child_nodes = &tree.aux_quad_nodes.back();
				child_nodes->InitializeAndCreateElementLists();

				// Remove all elements and reinsert them
				for (Element& temp_element : *temp_elements)
				{
					T* object = temp_element.object;
					AABB2D object_aabb = temp_element.aabb;
					tree.num_added_elements -= 1;

					child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb, optimizing);
				}

				RELEASE(temp_elements);
			}
		}

		bool IsLeaf() const
		{
			return element_count >= 0;
		}

		bool IsBranch() const
		{
			return element_count < 0;
		}

	public:
		int element_count = 0; // Leaf: number of elements. Branch: -1.
		union
		{
			Element* first_element = nullptr; // Leaf only: first element.
			QuadNode* child_nodes; // Branch only: child nodes index.
			std::list<Element>* temp_element_list;
		};
	};

	// Nodes are in groups of 4 so that only 1 pointer is needed
	struct QuadNode
	{
	public:
		void Initialize()
		{
			for (Node& node : nodes)
			{
				node.element_count = 0;
				node.first_element = nullptr;
			}
		}

		void InitializeAndCreateElementLists()
		{
			for (Node& node : nodes)
			{
				node.element_count = 0;
				node.temp_element_list = new std::list<Element>();
			}
		}

		void ReleaseElementLists()
		{
			for (Node& node : nodes)
			{
				if (node.IsLeaf())
				{
					RELEASE(node.temp_element_list);
				}
			}
		}

		void Add(Quadtree& tree, T* object, const AABB2D& object_aabb, unsigned depth, const AABB2D& node_aabb, bool optimizing)
		{
			vec2d center = node_aabb.minPoint + (node_aabb.maxPoint - node_aabb.minPoint) * 0.5f;

			AABB2D top_left_aabb = {{node_aabb.minPoint.x, center.y}, {center.x, node_aabb.maxPoint.y}};
			if (object_aabb.Intersects(top_left_aabb))
			{
				nodes[0].Add(tree, object, object_aabb, depth, top_left_aabb, optimizing);
			}

			AABB2D top_right_aabb = {{center.x, center.y}, {node_aabb.maxPoint.x, node_aabb.maxPoint.y}};
			if (object_aabb.Intersects(top_right_aabb))
			{
				nodes[1].Add(tree, object, object_aabb, depth, top_right_aabb, optimizing);
			}

			AABB2D bottom_left_aabb = {{node_aabb.minPoint.x, node_aabb.minPoint.y}, {center.x, center.y}};
			if (object_aabb.Intersects(bottom_left_aabb))
			{
				nodes[2].Add(tree, object, object_aabb, depth, bottom_left_aabb, optimizing);
			}

			AABB2D bottom_right_aabb = {{center.x, node_aabb.minPoint.y}, {node_aabb.maxPoint.x, center.y}};
			if (object_aabb.Intersects(bottom_right_aabb))
			{
				nodes[3].Add(tree, object, object_aabb, depth, bottom_right_aabb, optimizing);
			}
		}

		void Remove(Quadtree& tree, T* object)
		{
			for (Node& node : nodes)
			{
				node.Remove(tree, object);
			}
		}

	public:
		Node nodes[4];
	};

public:
	void Initialize(AABB2D quadtree_bounds, unsigned quadtree_max_depth, unsigned max_elements_per_node)
	{
		assert(quadtree_max_depth > 0);

		Clear();

		bounds = quadtree_bounds;
		max_depth = quadtree_max_depth;
		max_node_elements = max_elements_per_node;

		aux_root.temp_element_list = new std::list<Element>();
	}

	void Add(T* object, const AABB2D& object_aabb)
	{
		assert(!operative); // Tried to add an object to a locked quadtree

		aux_root.Add(*this, object, object_aabb, 1, bounds, false);
		added_objects.emplace_back(std::pair<T*, AABB2D>(object, object_aabb));
	}

	void Remove(T* object)
	{
		assert(operative); // Tried to remove an object from an unlocked quadtree

		root.Remove(*this, object);
	}

	void Optimize()
	{
		quad_nodes.Allocate(aux_quad_nodes.size());
		elements.Allocate(num_added_elements);

		for (std::pair<T*, AABB2D> pair : added_objects)
		{
			AddToPools(pair.first, pair.second);
		}

		for (QuadNode& quad_node : aux_quad_nodes)
		{
			quad_node.ReleaseElementLists();
		}

		if (aux_root.IsBranch())
		{
			aux_root.element_count = 0;
			aux_root.temp_element_list = nullptr;
		}
		else
		{
			aux_root.element_count = 0;
			RELEASE(aux_root.temp_element_list);
		}
		num_added_elements = 0;
		aux_quad_nodes.clear();
		added_objects.clear();

		operative = true;
	}

	bool IsOperative()
	{
		return operative;
	}

	void Clear()
	{
		bounds.minPoint.Set(0, 0);
		bounds.maxPoint.Set(0, 0);
		max_depth = 0;
		max_node_elements = 0;

		root.element_count = 0;
		root.first_element = nullptr;
		quad_nodes.Clear();
		elements.Clear();

		operative = false;

		for (QuadNode& quad_node : aux_quad_nodes)
		{
			quad_node.ReleaseElementLists();
		}

		if (aux_root.IsBranch())
		{
			aux_root.element_count = 0;
			aux_root.temp_element_list = nullptr;
		}
		else
		{
			aux_root.element_count = 0;
			RELEASE(aux_root.temp_element_list);
		}
		num_added_elements = 0;
		aux_quad_nodes.clear();
		added_objects.clear();
	}

public:
	AABB2D bounds = {{0, 0}, {0, 0}}; // Bounds of the quadtree. All elements should be contained inside this.
	unsigned max_depth = 0; // Max depth of the tree. Useful to avoid infinite divisions. This should be >= 1.
	unsigned max_node_elements = 0; // Max number of elements before a node is divided.

	Node root;
	Pool<QuadNode> quad_nodes;
	Pool<Element> elements;

protected:
	void AddToPools(T* object, const AABB2D& object_aabb)
	{
		root.Add(*this, object, object_aabb, 1, bounds, true);
	}

private:
	bool operative = false;

	Node aux_root;
	unsigned num_added_elements = 0;
	std::list<QuadNode> aux_quad_nodes;
	std::list<std::pair<T*, AABB2D>> added_objects;
};
