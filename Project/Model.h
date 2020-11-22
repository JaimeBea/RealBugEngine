#pragma once

#include "Mesh.h"

#include <vector>

class Model
{
public:
	bool Load(const char* file_name);
	void Release();

	void Draw() const;

public:
	std::vector<unsigned> materials;
	std::vector<Mesh> meshes;
};

