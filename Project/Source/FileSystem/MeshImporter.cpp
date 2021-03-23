#include "MeshImporter.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Resources/Mesh.h"
#include "Resources/GameObject.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"

#include "assimp/mesh.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include "Math/Quat.h"
#include "GL/glew.h"

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
	mesh->numBones = assimpMesh->mNumBones;
	mesh->attaches = std::vector<Mesh::Attach>(mesh->numVertices);
	mesh->bones = std::vector<Mesh::Bone>(mesh->numBones);

	// Save to custom format buffer
	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned bonesIDSize = sizeof(unsigned) * 4;
	unsigned weightsSize = sizeof(float) * 4;
	unsigned indexSize = sizeof(unsigned);
	unsigned attachSize = sizeof(Mesh::Attach);
	unsigned boneSize = sizeof(char) * FILENAME_MAX + sizeof(float) * 10;

	unsigned headerSize = sizeof(unsigned) * 3;
	unsigned vertexSize = positionSize + normalSize + uvSize + bonesIDSize + weightsSize;
	unsigned vertexBufferSize = vertexSize * mesh->numVertices;
	unsigned indexBufferSize = indexSize * mesh->numIndices;
	unsigned bonesBufferSize = boneSize * mesh->numBones;
	unsigned attachesBufferSize = attachSize * mesh->numVertices;

	size_t size = headerSize + vertexBufferSize + indexBufferSize + bonesBufferSize + attachesBufferSize;
	Buffer<char> buffer = Buffer<char>(size);
	char* cursor = buffer.Data();

	*((unsigned*) cursor) = mesh->numVertices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = mesh->numIndices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = mesh->numBones;
	cursor += sizeof(unsigned);

	std::vector<Mesh::Attach> attaches;
	attaches.resize(mesh->numVertices);
	for (unsigned i = 0; i < assimpMesh->mNumBones; ++i) {
		aiBone* aiBone = assimpMesh->mBones[i];

		*((unsigned*) cursor) = aiBone->mName.length;
		cursor += sizeof(unsigned);

		memcpy_s(cursor, aiBone->mName.length * sizeof(char), aiBone->mName.data, aiBone->mName.length * sizeof(char));
		cursor += FILENAME_MAX * sizeof(char);

		//Transform
		aiVector3D position, scaling;
		aiQuaternion rotation;
		aiBone->mOffsetMatrix.Decompose(scaling, rotation, position);

		//Position
		*((float*) cursor) = position.x;
		cursor += sizeof(float);
		*((float*) cursor) = position.y;
		cursor += sizeof(float);
		*((float*) cursor) = position.z;
		cursor += sizeof(float);

		// Scaling
		*((float*) cursor) = scaling.x;
		cursor += sizeof(float);
		*((float*) cursor) = scaling.y;
		cursor += sizeof(float);
		*((float*) cursor) = scaling.z;
		cursor += sizeof(float);

		// Rotation
		*((float*) cursor) = rotation.x;
		cursor += sizeof(float);
		*((float*) cursor) = rotation.y;
		cursor += sizeof(float);
		*((float*) cursor) = rotation.z;
		cursor += sizeof(float);
		*((float*) cursor) = rotation.w;
		cursor += sizeof(float);

		for (unsigned j = 0; j < aiBone->mNumWeights; j++) {
			aiVertexWeight vtxWeight = aiBone->mWeights[j];

			attaches[vtxWeight.mVertexId].bones[attaches[vtxWeight.mVertexId].numBones] = i;
			attaches[vtxWeight.mVertexId].weights[attaches[vtxWeight.mVertexId].numBones] = vtxWeight.mWeight;
			attaches[vtxWeight.mVertexId].numBones++;
		}
	}

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

		float weight = attaches[i].weights[0] + attaches[i].weights[1] + attaches[i].weights[2] + attaches[i].weights[3];

		*((unsigned*) cursor) = attaches[i].bones[0];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = attaches[i].bones[1];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = attaches[i].bones[2];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = attaches[i].bones[3];
		cursor += sizeof(unsigned);

		*((float*) cursor) = attaches[i].weights[0] / weight;
		cursor += sizeof(float);
		*((float*) cursor) = attaches[i].weights[1] / weight;
		cursor += sizeof(float);
		*((float*) cursor) = attaches[i].weights[2] / weight;
		cursor += sizeof(float);
		*((float*) cursor) = attaches[i].weights[3] / weight;
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

void MeshImporter::LoadMesh(Mesh* mesh, std::unordered_map<std::string, GameObject*>& bones) {
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
	mesh->numBones = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned bonesIDSize = sizeof(unsigned) * 4;
	unsigned weightsSize = sizeof(float) * 4;
	unsigned indexSize = sizeof(unsigned);

	unsigned vertexSize = positionSize + normalSize + uvSize + bonesIDSize + weightsSize;
	unsigned vertexBufferSize = vertexSize * mesh->numVertices;
	unsigned indexBufferSize = indexSize * mesh->numIndices;

	mesh->bones.resize(mesh->numBones);

	// Bones
	for (unsigned i = 0; i < mesh->numBones; i++) {
		float3 position, scaling;
		Quat rotation;

		unsigned lengthName = *((unsigned*) cursor);
		cursor += sizeof(unsigned);

		char* name = (char*) malloc((lengthName + 1) * sizeof(char));

		memcpy_s(name, lengthName * sizeof(char), cursor, lengthName * sizeof(char));
		cursor += FILENAME_MAX * sizeof(char);

		name[lengthName] = '\0';

		// Init the bones map only with the Key. In the scene importer, add the pointers to the GameObject-Bone.
		bones.emplace(name, nullptr);

		// Translation
		position.x = *((float*) cursor);
		cursor += sizeof(float);
		position.y = *((float*) cursor);
		cursor += sizeof(float);
		position.z = *((float*) cursor);
		cursor += sizeof(float);

		// Scaling
		scaling.x = *((float*) cursor);
		cursor += sizeof(float);
		scaling.y = *((float*) cursor);
		cursor += sizeof(float);
		scaling.z = *((float*) cursor);
		cursor += sizeof(float);

		// Rotation
		rotation.x = *((float*) cursor);
		cursor += sizeof(float);
		rotation.y = *((float*) cursor);
		cursor += sizeof(float);
		rotation.z = *((float*) cursor);
		cursor += sizeof(float);
		rotation.w = *((float*) cursor);
		cursor += sizeof(float);

		mesh->bones[i].transform = float4x4::FromTRS(position, rotation, scaling);
		mesh->bones[i].boneName = name;

		free(name);
	}

	// Vertices
	mesh->vertices = (float*) malloc(vertexBufferSize);
	memcpy_s(mesh->vertices, vertexBufferSize, (float*) cursor, vertexBufferSize);
	cursor += vertexBufferSize;

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
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, mesh->vertices, (mesh->numBones > 0) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indices, GL_STATIC_DRAW);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) positionSize);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*) (positionSize + normalSize));
	glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, vertexSize, (void*) (positionSize + normalSize + uvSize));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, vertexSize, (void*) (positionSize + normalSize + uvSize + bonesIDSize));

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
	unsigned numBones = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	// Bones
	for (unsigned i = 0; i < numBones; i++) {
		// filename size
		cursor += sizeof(unsigned);
		// filename
		cursor += FILENAME_MAX * sizeof(char);
		// translation, scaling, rotation
		cursor += sizeof(float) * 10;
	}

	// Vertices
	std::vector<float3> vertices;
	for (unsigned i = 0; i < numVertices; i++) {
		float vertex[3] = {};
		vertex[0] = *((float*) cursor);
		cursor += sizeof(float);
		vertex[1] = *((float*) cursor);
		cursor += sizeof(float);
		vertex[2] = *((float*) cursor);
		cursor += sizeof(float) * 14;
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

	free(mesh->vertices);

	mesh->attaches.clear();
	mesh->bones.clear();

	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
}