#include "MeshImporter.h"

#include "Application.h"
#include "Logging.h"
#include "ModuleResources.h"
#include "ModuleFiles.h"
#include "Mesh.h"
#include "Buffer.h"

#include "assimp/mesh.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include "GL/glew.h"
#include <list>
#include <vector>

Mesh* MeshImporter::ImportMesh(const aiMesh* ai_mesh)
{
	// Timer to measure importing a mesh
	MSTimer timer;
	timer.Start();

	// Create mesh
	Mesh* mesh = App->resources->meshes.Obtain();
	mesh->num_vertices = ai_mesh->mNumVertices;
	mesh->num_indices = ai_mesh->mNumFaces * 3;
	mesh->material_index = ai_mesh->mMaterialIndex;

	// Save to custom format buffer
	unsigned position_size = sizeof(float) * 3;
	unsigned normal_size = sizeof(float) * 3;
	unsigned uv_size = sizeof(float) * 2;
	unsigned index_size = sizeof(unsigned);

	unsigned header_size = sizeof(unsigned) * 2;
	unsigned vertex_size = position_size + normal_size + uv_size;
	unsigned vertex_buffer_size = vertex_size * mesh->num_vertices;
	unsigned index_buffer_size = index_size * mesh->num_indices;

	size_t size = header_size + vertex_buffer_size + index_buffer_size;
	Buffer<char> buffer = Buffer<char>(size);
	char* cursor = buffer.Data();

	*((unsigned*) cursor) = mesh->num_vertices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = mesh->num_indices;
	cursor += sizeof(unsigned);

	for (unsigned i = 0; i < ai_mesh->mNumVertices; ++i)
	{
		aiVector3D& vertex = ai_mesh->mVertices[i];
		aiVector3D& normal = ai_mesh->mNormals[i];
		aiVector3D* texture_coords = ai_mesh->mTextureCoords[0];

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
		*((float*) cursor) = texture_coords != nullptr ? texture_coords[i].x : 0;
		cursor += sizeof(float);
		*((float*) cursor) = texture_coords != nullptr ? texture_coords[i].y : 0;
		cursor += sizeof(float);
	}

	for (unsigned i = 0; i < ai_mesh->mNumFaces; ++i)
	{
		aiFace& ai_face = ai_mesh->mFaces[i];

		// Assume triangles = 3 indices per face
		if (ai_face.mNumIndices != 3)
		{
			LOG("Found a face with %i vertices. Discarded.", ai_face.mNumIndices);

			*((unsigned*) cursor) = 0;
			cursor += sizeof(unsigned);
			*((unsigned*) cursor) = 0;
			cursor += sizeof(unsigned);
			*((unsigned*) cursor) = 0;
			cursor += sizeof(unsigned);
			continue;
		}

		*((unsigned*) cursor) = ai_face.mIndices[0];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = ai_face.mIndices[1];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = ai_face.mIndices[2];
		cursor += sizeof(unsigned);
	}

	// Save buffer to file
	mesh->file_name = ai_mesh->mName.C_Str();
	std::string file_path = std::string(MESHES_PATH) + "/" + mesh->file_name + MESH_EXTENSION;
	LOG("Saving mesh to \"%s\".", file_path.c_str());
	App->files->Save(file_path.c_str(), buffer);

	unsigned time_ms = timer.Stop();
	LOG("Mesh imported in %ums", time_ms);
	return mesh;
}

void MeshImporter::LoadMesh(Mesh* mesh)
{
	if (mesh == nullptr) return;

	// Timer to measure loading a mesh
	MSTimer timer;
	timer.Start();

	std::string file_path = std::string(MESHES_PATH) + "/" + mesh->file_name + MESH_EXTENSION;

	LOG("Loading mesh from path: \"%s\".", file_path.c_str());

	// Load file
	Buffer<char> buffer = App->files->Load(file_path.c_str());
	char* cursor = buffer.Data();

	// Header
	mesh->num_vertices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);
	mesh->num_indices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	unsigned position_size = sizeof(float) * 3;
	unsigned normal_size = sizeof(float) * 3;
	unsigned uv_size = sizeof(float) * 2;
	unsigned index_size = sizeof(unsigned);

	unsigned vertex_size = position_size + normal_size + uv_size;
	unsigned vertex_buffer_size = vertex_size * mesh->num_vertices;
	unsigned index_buffer_size = index_size * mesh->num_indices;

	// Vertices
	float* vertices = (float*) cursor;
	cursor += vertex_size * mesh->num_vertices;

	// Indices
	unsigned* indices = (unsigned*) cursor;

	LOG("Loading %i vertices...", mesh->num_vertices);

	// Create VAO
	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glGenBuffers(1, &mesh->ebo);

	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	// Load VBO
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertices, GL_STATIC_DRAW);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, indices, GL_STATIC_DRAW);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*) position_size);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*) (position_size + normal_size));

	// Unbind VAO
	glBindVertexArray(0);

	unsigned time_ms = timer.Stop();
	LOG("Mesh loaded in %ums", time_ms);
}

void MeshImporter::ExtractMeshTriangles(Mesh* mesh, std::list<Triangle>& triangles, float4x4 model)
{
	std::string file_path = std::string(MESHES_PATH) + "/" + mesh->file_name + MESH_EXTENSION;

	// Load file
	Buffer<char> buffer = App->files->Load(file_path.c_str());
	char* cursor = buffer.Data();

	//Skip header
	cursor += sizeof(unsigned) * 2;

	// Vertices
	std::vector<float3> vertices;
	for (int i = 0; i < mesh->num_vertices; i++)
	{
		float vertex[3] = {};
		vertex[0] = *(float*) cursor;
		cursor += sizeof(float);
		vertex[1] = *(float*) cursor;
		cursor += sizeof(float);
		vertex[2] = *(float*) cursor;
		cursor += sizeof(float) * 6;
		vertices.push_back((model * float4(vertex[0], vertex[1], vertex[2], 1)).xyz());
	}

	for (int i = 0; i < mesh->num_indices / 3; i++)
	{
		int triange_indices[3] = {};
		triange_indices[0] = *(unsigned*) cursor;
		cursor += sizeof(unsigned);
		triange_indices[1] = *(unsigned*) cursor;
		cursor += sizeof(unsigned);
		triange_indices[2] = *(unsigned*) cursor;
		cursor += sizeof(unsigned);
		triangles.push_back(Triangle(vertices[triange_indices[0]], vertices[triange_indices[1]], vertices[triange_indices[2]]));
	}
}

void MeshImporter::UnloadMesh(Mesh* mesh)
{
	if (!mesh->vao) return;

	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
}