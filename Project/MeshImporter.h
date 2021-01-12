#pragma once

#include "Geometry/Triangle.h"
#include <list>

class Mesh;
struct aiMesh;

namespace MeshImporter
{
	Mesh* ImportMesh(const aiMesh* ai_mesh);
	void LoadMesh(Mesh* mesh);
	void ExtractMeshTriangles(Mesh* mesh, std::list<Triangle>& triangles);
	void UnloadMesh(Mesh* mesh);
};
