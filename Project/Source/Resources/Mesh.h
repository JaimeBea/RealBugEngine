#pragma once

#include <string>
#include <vector>

#include "Math/float4x4.h"

class Mesh {
public:
	struct Bone {
		float4x4 transform;
		std::string boneName;
	};

	struct Attach {
		unsigned numBones = 0;
		unsigned bones[4];
		float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	};

public:
	std::string fileName = "";
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned materialIndex = 0;
	unsigned numBones = 0;
	float* vertices;
	std::vector<Mesh::Attach> attaches; // For each vertex, we store the bone indices and wights that this vertex is related.
	std::vector<Mesh::Bone> bones;		// The bones and it's transform from a Mesh
};