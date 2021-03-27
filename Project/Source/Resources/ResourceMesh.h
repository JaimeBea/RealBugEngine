#pragma once

#include "Resources/Resource.h"

#include "Geometry/Triangle.h"
#include "Math/float4x4.h"
#include <string>
#include <vector>

class ResourceMesh : public Resource {
public:
	REGISTER_RESOURCE(ResourceMesh, ResourceType::MESH);

	void Load() override;
	void Unload() override;

	std::vector<Triangle> ExtractTriangles(const float4x4& modelMatrix) const;

public:
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
};