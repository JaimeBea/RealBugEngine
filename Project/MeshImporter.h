#pragma once

#include "Math/float4x4.h"
#include "Geometry/Triangle.h"
#include <list>

class Mesh;
struct aiMesh;

namespace MeshImporter
{
	Mesh* ImportMesh(const aiMesh* ai_mesh);
	void LoadMesh(Mesh* mesh);
	void ExtractMeshTriangles(Mesh* mesh, std::list<Triangle>& triangles, float4x4 model);
	void UnloadMesh(Mesh* mesh);
}; // namespace MeshImporter