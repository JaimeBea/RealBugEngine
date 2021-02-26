#include "ModelImporter.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/MSTimer.h"
#include "Resources/ResourceMesh.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include "assimp/mesh.h"

ResourceMesh* ImportMesh(const char* modelFilePath, JsonValue jMeta, const aiMesh* assimpMesh, unsigned resourceIndex) {
	// Timer to measure importing a mesh
	MSTimer timer;
	timer.Start();

	unsigned numVertices = assimpMesh->mNumVertices;
	unsigned numIndices = assimpMesh->mNumFaces * 3;
	unsigned materialIndex = assimpMesh->mMaterialIndex;

	// Save to custom format buffer
	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned indexSize = sizeof(unsigned);

	unsigned headerSize = sizeof(unsigned) * 2;
	unsigned vertexSize = positionSize + normalSize + uvSize;
	unsigned vertexBufferSize = vertexSize * numVertices;
	unsigned indexBufferSize = indexSize * numIndices;

	size_t size = headerSize + vertexBufferSize + indexBufferSize;
	Buffer<char> buffer = Buffer<char>(size);
	char* cursor = buffer.Data();

	*((unsigned*) cursor) = numVertices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = numIndices;
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

		// Assume triangles have 3 indices per face
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

	// Create mesh
	ResourceMesh* mesh = App->resources->CreateResource<ResourceMesh>(modelFilePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[resourceIndex] = mesh->GetId();

	// Save buffer to file
	const std::string& resourceFilePath = mesh->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save mesh resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("Mesh imported in %ums", timeMs);
	return mesh;
}

bool ModelImporter::ImportModel(const char* filePath, JsonValue jMeta) {
	// TODO: (Mesh resource) Import models
	// TODO: (Material resource) Import models
	// TODO: (Texture resource) Import models
	return false;
}