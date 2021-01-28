#pragma once

#include "Math/float4x4.h"
#include "Geometry/Triangle.h"
#include <vector>

class Mesh;
struct aiMesh;

namespace MeshImporter {
	Mesh* ImportMesh(const aiMesh* assimpMesh, unsigned index);
	void LoadMesh(Mesh* mesh);
	std::vector<Triangle> ExtractMeshTriangles(Mesh* mesh, const float4x4& model);
	void UnloadMesh(Mesh* mesh);
}; // namespace MeshImporter