#pragma once

#include "Math/myassert.h"
#include "Geometry/AABB2D.h"
#include "Pool.h"

template<typename T>
class Quadtree
{
public:
	// The elements in a node are linked
	class Element
	{
	public:
		T* object = nullptr;
		Element* next = nullptr;
	};

	// Nodes are as small as possible (8 bytes) to reduce memory usage and cache efficiency
	class QuadNode;
	class Node
	{
	public:
		void Add(Quadtree& tree, T* object, const AABB2D& object_aabb, unsigned depth, const AABB2D& node_aabb)
		{
			if (IsBranch())
			{
				// Branch
				child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb);
			}
			else if (depth == tree.max_depth || element_count < tree.max_node_elements)
			{
				// Leaf that can't split or leaf with space
				Element* new_first_element = tree.elements.Obtain();
				new_first_element->object = object;
				new_first_element->next = first_element;
				first_element = new_first_element;
				element_count += 1;
			}
			else
			{
				// Leaf with no space that can split
				Split(tree, object_aabb, depth, node_aabb);
				child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb);
			}
		}

		void Remove(Quadtree& tree, T* object)
		{
			if (IsBranch())
			{
				// Branch
				child_nodes->Remove(tree, object);
			}
			else
			{
				// Leaf
				Element** address = &first_element;
				while (*address != nullptr)
				{
					if ((*address)->object == object)
					{
						tree.elements.Release(*address);
						*address = (*address)->next;
						return; // Elements can't repeat within the same node
					}

					address = &(*address)->next;
				}
			}
		}

		void Split(Quadtree& tree, const AABB2D& object_aabb, unsigned depth, const AABB2D& node_aabb)
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
				Element* next_element = element->next;
				tree.elements.Release(element);
				element = next_element;

				child_nodes->Add(tree, object, object_aabb, depth + 1, node_aabb);
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

		void Add(Quadtree& tree, T* object, const AABB2D& object_aabb, unsigned depth, const AABB2D& node_aabb)
		{
			vec2d center = (node_aabb.maxPoint - node_aabb.minPoint) * 0.5f;

			Node& top_left = nodes[0];
			AABB2D top_left_aabb = {{node_aabb.minPoint.x, center.y}, {center.x, node_aabb.maxPoint.y}};
			if (object_aabb.Intersects(top_left_aabb))
			{
				top_left.Add(tree, object, object_aabb, depth, top_left_aabb);
			}

			Node& top_right = nodes[1];
			AABB2D top_right_aabb = {{center.x, center.y}, {node_aabb.maxPoint.x, node_aabb.maxPoint.y}};
			if (object_aabb.Intersects(top_right_aabb))
			{
				top_right.Add(tree, object, object_aabb, depth, top_right_aabb);
			}

			Node& bottom_left = nodes[2];
			AABB2D bottom_left_aabb = {{node_aabb.minPoint.x, node_aabb.minPoint.y}, {center.x, center.y}};
			if (object_aabb.Intersects(bottom_left_aabb))
			{
				bottom_left.Add(tree, object, object_aabb, depth, bottom_left_aabb);
			}

			Node& bottom_right = nodes[3];
			AABB2D bottom_right_aabb = {{center.x, node_aabb.minPoint.y}, {node_aabb.maxPoint.x, center.y}};
			if (object_aabb.Intersects(bottom_right_aabb))
			{
				bottom_right.Add(tree, object, object_aabb, depth, bottom_right_aabb);
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
	void Initialize(AABB2D quadtree_bounds, unsigned quadtree_max_depth, unsigned max_elements_per_node, unsigned max_elements)
	{
		assert(quadtree_max_depth > 0);

		bounds = quadtree_bounds;
		max_depth = quadtree_max_depth;
		max_node_elements = max_elements_per_node;
		unsigned node_amount = ((int) Pow((float) 4, (float) max_depth) - 1) / 3; // Calculates the maximum amount of nodes depending on max depth (1, 5, 21...)
		quad_nodes.Allocate((node_amount - 1) / 4);
		elements.Allocate(max_elements);
	}

	void Add(T* object, const AABB2D& object_aabb)
	{
		root.Add(*this, object, object_aabb, 1, bounds);
	}

	void Remove(T* object)
	{
		root.Remove(*this, object);
	}

	void Clear()
	{
		root.element_count = 0;
		root.first_element = nullptr;
		quad_nodes.ReleaseAll();
		elements.ReleaseAll();
	}

public:
	AABB2D bounds = {{0, 0}, {0, 0}}; // Bounds of the quadtree. All elements should be contained inside this.
	unsigned max_depth = 0; // Max depth of the tree. Useful to avoid infinite divisions. This should be >= 1.
	unsigned max_node_elements = 0; // Max number of elements before a node is divided.

	Node root; // Root of the quadtree.
	Pool<QuadNode> quad_nodes;
	Pool<Element> elements;
};
