#pragma once

#include "Math/float4x4.h"
#include "Geometry/Triangle.h"
#include <vector>
#include <unordered_map>

class Mesh;
class GameObject;
struct aiMesh;

namespace MeshImporter {
	Mesh* ImportMesh(const aiMesh* assimpMesh, unsigned index);
	void LoadMesh(Mesh* mesh, std::unordered_map<std::string, GameObject*>& bones);
	std::vector<Triangle> ExtractMeshTriangles(Mesh* mesh, const float4x4& model);
	void UnloadMesh(Mesh* mesh);
}; // namespace MeshImporter