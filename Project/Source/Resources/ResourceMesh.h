#pragma once

#include "Resources/Resource.h"

#include <string>

class ResourceMesh : public Resource {
public:
	ResourceMesh(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

public:
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned materialIndex = 0;
};