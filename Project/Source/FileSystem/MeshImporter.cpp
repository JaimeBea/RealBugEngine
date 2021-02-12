#include "MeshImporter.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Resources/Mesh.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"

#include "assimp/mesh.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include "GL/glew.h"
#include <list>
#include <vector>

#include "Utils/Leaks.h"

Mesh* MeshImporter::ImportMesh(const aiMesh* assimpMesh, unsigned index) {
	// Timer to measure importing a mesh
	MSTimer timer;
	timer.Start();

	// Create mesh
	Mesh* mesh = App->resources->meshes.Obtain();
	mesh->numVertices = assimpMesh->mNumVertices;
	mesh->numIndices = assimpMesh->mNumFaces * 3;
	mesh->materialIndex = assimpMesh->mMaterialIndex;

	// Save to custom format buffer
	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned indexSize = sizeof(unsigned);

	unsigned headerSize = sizeof(unsigned) * 2;
	unsigned vertexSize = positionSize + normalSize + uvSize;
	unsigned vertexBufferSize = vertexSize * mesh->numVertices;
	unsigned indexBufferSize = indexSize * mesh->numIndices;

	size_t size = headerSize + vertexBufferSize + indexBufferSize;
	Buffer<char> buffer = Buffer<char>(size);
	char* cursor = buffer.Data();

	*((unsigned*) cursor) = mesh->numVertices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = mesh->numIndices;
	cursor += sizeof(unsigned);

	for (unsigned i = 0; i < assimpMesh->mNumVertices; ++i) {
		aiVector3D& vertex = assimpMesh->mVertices[i];
		aiVector3D& normal = assimpMesh->mNormals[i];
		aiVector3D* textureCoords = assimpMesh->mTextureCoords[0];

		*((float*) cursor) = vertex.x;
		cursor += sizeof(float);
		*((float*) cursor) = vertex.y;
		cursor += sizeof(float);
		*((float*) cursor) = vertex.z;
		cursor += sizeof(float);
		*((float*) cursor) = normal.x;
		cursor += sizeof(float);
		*((float*) cursor) = normal.y;
		cursor += sizeof(float);
		*((float*) cursor) = normal.z;
		cursor += sizeof(float);
		*((float*) cursor) = textureCoords != nullptr ? textureCoords[i].x : 0;
		cursor += sizeof(float);
		*((float*) cursor) = textureCoords != nullptr ? textureCoords[i].y : 0;
		cursor += sizeof(float);
	}

	for (unsigned i = 0; i < assimpMesh->mNumFaces; ++i) {
		aiFace& assimpFace = assimpMesh->mFaces[i];

		// Assume triangles = 3 indices per face
		if (assimpFace.mNumIndices != 3) {
			LOG("Found a face with %i vertices. Discarded.", assimpFace.mNumIndices);

			*((unsigned*) cursor) = 0;
			cursor += sizeof(unsigned);
			*((unsigned*) cursor) = 0;
			cursor += sizeof(unsigned);
			*((unsigned*) cursor) = 0;
			cursor += sizeof(unsigned);
			continue;
		}

		*((unsigned*) cursor) = assimpFace.mIndices[0];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = assimpFace.mIndices[1];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = assimpFace.mIndices[2];
		cursor += sizeof(unsigned);
	}

	// Save buffer to file

	std::string fileName = std::string(assimpMesh->mName.C_Str()) + std::to_string(index);
	mesh->fileName = fileName;
	std::string filePath = std::string(MESHES_PATH) + "/" + fileName + MESH_EXTENSION;
	LOG("Saving mesh to \"%s\".", filePath.c_str());
	App->files->Save(filePath.c_str(), buffer);

	unsigned timeMs = timer.Stop();
	LOG("Mesh imported in %ums", timeMs);
	return mesh;
}

void MeshImporter::LoadMesh(Mesh* mesh) {
	if (mesh == nullptr) return;

	// Timer to measure loading a mesh
	MSTimer timer;
	timer.Start();

	std::string filePath = std::string(MESHES_PATH) + "/" + mesh->fileName + MESH_EXTENSION;

	LOG("Loading mesh from path: \"%s\".", filePath.c_str());

	// Load file
	Buffer<char> buffer = App->files->Load(filePath.c_str());
	char* cursor = buffer.Data();

	// Header
	mesh->numVertices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);
	mesh->numIndices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned indexSize = sizeof(unsigned);

	unsigned vertexSize = positionSize + normalSize + uvSize;
	unsigned vertexBufferSize = vertexSize * mesh->numVertices;
	unsigned indexBufferSize = indexSize * mesh->numIndices;

	// Vertices
	float* vertices = (float*) cursor;
	cursor += vertexSize * mesh->numVertices;

	// Indices
	unsigned* indices = (unsigned*) cursor;

	LOG("Loading %i vertices...", mesh->numVertices);

	// Create VAO
	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glGenBuffers(1, &mesh->ebo);

	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	// Load VBO
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices, GL_STATIC_DRAW);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indices, GL_STATIC_DRAW);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) positionSize);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*) (positionSize + normalSize));

	// Unbind VAO
	glBindVertexArray(0);

	unsigned timeMs = timer.Stop();
	LOG("Mesh loaded in %ums", timeMs);
}

std::vector<Triangle> MeshImporter::ExtractMeshTriangles(Mesh* mesh, const float4x4& model) {
	std::string filePath = std::string(MESHES_PATH) + "/" + mesh->fileName + MESH_EXTENSION;

	// Load file
	Buffer<char> buffer = App->files->Load(filePath.c_str());
	char* cursor = buffer.Data();

	// Header
	unsigned numVertices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);
	unsigned numIndices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	// Vertices
	std::vector<float3> vertices;
	for (unsigned i = 0; i < numVertices; i++) {
		float vertex[3] = {};
		vertex[0] = *((float*) cursor);
		cursor += sizeof(float);
		vertex[1] = *((float*) cursor);
		cursor += sizeof(float);
		vertex[2] = *((float*) cursor);
		cursor += sizeof(float) * 6;
		vertices.push_back((model * float4(vertex[0], vertex[1], vertex[2], 1)).xyz());
	}

	std::vector<Triangle> triangles;
	triangles.reserve(numIndices / 3);
	for (unsigned i = 0; i < numIndices / 3; i++) {
		unsigned triangeIndices[3] = {};
		triangeIndices[0] = *((unsigned*) cursor);
		cursor += sizeof(unsigned);
		triangeIndices[1] = *((unsigned*) cursor);
		cursor += sizeof(unsigned);
		triangeIndices[2] = *((unsigned*) cursor);
		cursor += sizeof(unsigned);
		triangles.push_back(Triangle(vertices[triangeIndices[0]], vertices[triangeIndices[1]], vertices[triangeIndices[2]]));
	}

	return triangles;
}

void MeshImporter::UnloadMesh(Mesh* mesh) {
	if (!mesh->vao) return;

	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
}