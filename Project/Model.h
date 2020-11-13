#pragma once

#include "Mesh.h"

#include <vector>

struct aiScene;

class Model
{
public:
	void Load(const char* file_name);
	void Release();

	void Draw() const;

private:
	std::vector<unsigned> materials;
	std::vector<Mesh> meshes;
};

