#pragma once

#include "Mesh.h"

#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <vector>

class Model
{
public:
	bool Load(const char* file_name);
	void Release();

	void Draw(const float4x4& model_matrix) const;

public:
	std::vector<unsigned> materials;
	std::vector<Mesh> meshes;
	Sphere bounding_sphere;
};

