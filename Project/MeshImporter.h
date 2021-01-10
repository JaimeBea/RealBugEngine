#pragma once

class Mesh;
struct aiMesh;

namespace MeshImporter
{
	Mesh* ImportMesh(const aiMesh* ai_mesh);
	void LoadMesh(Mesh* mesh);
	void UnloadMesh(Mesh* mesh);
};
