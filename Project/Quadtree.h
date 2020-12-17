#pragma once

#include "GameObject.h"
#include "Pool.h"

#include "Math/MathFunc.h"
#include "Math/myassert.h"
#include "Geometry/AABB2D.h"

class Quadtree
{
public:
	void Initialize(AABB2D quadtree_bounds, unsigned quadtree_max_depth, unsigned max_elements_per_node, unsigned max_element_amount)
	{
		assert(quadtree_max_depth > 0);

		bounds = quadtree_bounds;
		max_depth = quadtree_max_depth;
		max_node_elements = max_elements_per_node;
		unsigned node_amount = ((int) Pow((float) 4, (float) max_depth) - 1) / 3; // Calculates the maximum amount of nodes depending on max depth (1, 5, 21...)
		child_nodes.Allocate((node_amount - 1) / 4);
		elements.Allocate(max_element_amount);
	}

	void Add(GameObject* game_object)
	{

	}

private:
	struct Node
	{
		int element_count = 0; // Leaf: number of elements. Branch: -1.
		void* first = nullptr; // Leaf: first element ID. Branch: child nodes.
	};

	// Nodes are in groups of 4 so that only 1 pointer is needed
	struct ChildNodes
	{
		Node nodes[4];
	};

	// The elements in a node are linked
	struct NodeElement
	{
		GameObject* game_object = nullptr;
		NodeElement* next = nullptr;
	};

private:
	AABB2D bounds = AABB2D(vec2d(0, 0), vec2d(0, 0)); // Bounds of the quadtree. All elements should be contained inside this.
	unsigned max_depth = 0; // Max depth of the tree. Useful to avoid infinite divisions. This should be >= 1.
	unsigned max_node_elements = 0; // Max number of elements before a node is divided.

	Node root = Node(); // Root of the quadtree.
	Pool<ChildNodes> child_nodes = Pool<ChildNodes>();
	Pool<NodeElement> elements = Pool<NodeElement>();
};

