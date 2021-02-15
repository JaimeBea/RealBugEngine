#pragma once

#include "Resources/Resource.h"

#include <string>

class ResourceMesh : public Resource {
public:
	ResourceMesh(UID id, std::string localFilePath);

	void Import() override;
	void Delete() override;

public:
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned materialIndex = 0;

protected:
	void Load() override;
	void Unload() override;
};