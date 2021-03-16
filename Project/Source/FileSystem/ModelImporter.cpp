#include "ModelImporter.h"

#include "Application.h"
#include "GameObject.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/MSTimer.h"
#include "Utils/FileDialog.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentMeshRenderer.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"


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

void ImportNode(const char* modelFilePath, JsonValue jMeta, const aiScene* assimpScene, const std::vector<ResourceMaterial*>& materials, const aiNode* node, GameObject* parent, const float4x4& accumulatedTransform) {
	std::string name = node->mName.C_Str();
	LOG("Importing node: \"%s\"", name.c_str());

	if (name.find("$AssimpFbx$") != std::string::npos) { // Auxiliary node
		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			const float4x4& transform = accumulatedTransform * (*(float4x4*) &node->mTransformation);
			ImportNode(modelFilePath, jMeta, assimpScene, materials, node->mChildren[i], parent, transform);
		}
	} else { // Normal node
		// Create GameObject
		GameObject* gameObject = App->scene->CreateGameObject(parent);

		// Load name
		gameObject->name = name;

		// Load transform
		ComponentTransform* transform = gameObject->CreateComponent<ComponentTransform>();
		const float4x4& matrix = accumulatedTransform * (*(float4x4*) &node->mTransformation);
		float3 position;
		Quat rotation;
		float3 scale;
		matrix.Decompose(position, rotation, scale);
		transform->SetPosition(position);
		transform->SetRotation(rotation);
		transform->SetScale(scale);
		transform->CalculateGlobalMatrix();
		LOG("Transform: (%f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f)", position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, rotation.w, scale.x, scale.y, scale.z);

		// Save min and max points
		vec minPoint = vec(FLOAT_INF, FLOAT_INF, FLOAT_INF);
		vec maxPoint = vec(-FLOAT_INF, -FLOAT_INF, -FLOAT_INF);

		// Load meshes
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			LOG("Importing mesh %i", i);
			aiMesh* assimpMesh = assimpScene->mMeshes[node->mMeshes[i]];

			ComponentMeshRenderer* mesh = gameObject->CreateComponent<ComponentMeshRenderer>();
			mesh->mesh = ImportMesh(modelFilePath, jMeta, assimpMesh, i);

			// TODO: Move mesh loading to a better place
			mesh->mesh->Load();

			if (materials.size() > 0) {
				if (assimpMesh->mMaterialIndex >= materials.size()) {
					mesh->material = materials.front();
					LOG("Invalid material found", assimpMesh->mMaterialIndex);
				} else {
					mesh->material = materials[assimpMesh->mMaterialIndex];
				}
			}

			// Update min and max points
			for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
				aiVector3D vertex = assimpMesh->mVertices[j];
				if (vertex.x < minPoint.x) minPoint.x = vertex.x;
				if (vertex.y < minPoint.y) minPoint.y = vertex.y;
				if (vertex.z < minPoint.z) minPoint.z = vertex.z;
				if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
				if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
				if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
			}
		}

		// Create bounding box
		if (minPoint.x < maxPoint.x) {
			ComponentBoundingBox* boundingBox = gameObject->CreateComponent<ComponentBoundingBox>();
			boundingBox->SetLocalBoundingBox(AABB(minPoint, maxPoint));
			boundingBox->CalculateWorldBoundingBox();
		}

		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			ImportNode(modelFilePath, jMeta, assimpScene, materials, node->mChildren[i], gameObject, float4x4::identity);
		}
	}
}

bool ModelImporter::ImportModel(const char* filePath, JsonValue jMeta) {
	// TODO: (Mesh resource) Import models

	// Timer to measure importing a scene
	MSTimer timer;
	timer.Start();

	// Check for extension support
	std::string extension = FileDialog::GetFileExtension(filePath);
	if (!aiIsExtensionSupported(extension.c_str())) {
		LOG("Extension is not supported by assimp: \"%s\".", extension);
		return false;
	}

	// Import scene
	LOG("Importing scene from path: \"%s\".", filePath);
	const aiScene* assimpScene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
	DEFER {
		aiReleaseImport(assimpScene);
	};
	if (!assimpScene) {
		LOG("Error importing scene: %s", filePath, aiGetErrorString());
		return false;
	}

	// TODO: (Material resource) Import models
	// TODO: (Texture resource) Import models
	/*
	// Load materials
	LOG("Importing %i materials...", assimpScene->mNumMaterials);
	std::vector<Material> materials;
	materials.reserve(assimpScene->mNumMaterials);
	for (unsigned int i = 0; i < assimpScene->mNumMaterials; ++i) {
		LOG("Loading material %i...", i);
		aiMaterial* assimpMaterial = assimpScene->mMaterials[i];
		aiString materialFilePath;
		aiTextureMapping mapping;
		aiColor4D color;
		unsigned uvIndex;
		Material material;
		if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &materialFilePath, &mapping, &uvIndex) == AI_SUCCESS) {
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uvIndex == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import diffuse texture...");
			Texture* texture = TextureImporter::ImportTexture(materialFilePath.C_Str());

			// Try to load relative to the model folder
			if (texture == nullptr) {
				LOG("Trying to import texture relative to model folder...");
				std::string modelFolderPath = App->files->GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				texture = TextureImporter::ImportTexture(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFile = App->files->GetFileNameAndExtension(materialFilePath.C_Str());
				std::string texturesFolderMaterialFileDir = std::string(TEXTURES_PATH) + "/" + materialFile;
				texture = TextureImporter::ImportTexture(texturesFolderMaterialFileDir.c_str());
			}

			if (texture == nullptr) {
				LOG("Unable to find diffuse texture file.");
			} else {
				LOG("Diffuse texture imported successfuly.");
				material.hasDiffuseMap = true;
				material.diffuseMap = texture;
				// TODO: Move load to a better place
				TextureImporter::LoadTexture(texture);
			}
		} else {
			LOG("Diffuse texture not found.");
		}

		if (assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &materialFilePath, &mapping, &uvIndex) == AI_SUCCESS) {
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uvIndex == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import specular texture...");
			Texture* texture = TextureImporter::ImportTexture(materialFilePath.C_Str());

			// Try to load relative to the model folder
			if (texture == nullptr) {
				LOG("Trying to import texture relative to model folder...");
				std::string modelFolderPath = App->files->GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				texture = TextureImporter::ImportTexture(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFileName = App->files->GetFileName(materialFilePath.C_Str());
				std::string texturesFolderMaterialFileDir = std::string(TEXTURES_PATH) + "/" + materialFileName + TEXTURE_EXTENSION;
				texture = TextureImporter::ImportTexture(texturesFolderMaterialFileDir.c_str());
			}

			if (texture == nullptr) {
				LOG("Unable to find specular texture file.");
			} else {
				LOG("Specular texture imported successfuly.");
				material.hasSpecularMap = true;
				material.specularMap = texture;
				// TODO: Move load to a better place
				TextureImporter::LoadTexture(texture);
			}
		} else {
			LOG("Specular texture not found.");
		}

		assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, material.diffuseColor);
		assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, material.specularColor);
		assimpMaterial->Get(AI_MATKEY_SHININESS, material.shininess);

		LOG("Material imported.");
		materials.push_back(material);
	}

	// Create scene tree
	LOG("Importing scene tree.");
	ImportNode(filePath, jMeta, assimpScene, materials, assimpScene->mRootNode, parent, float4x4::identity);
	*/

	unsigned timeMs = timer.Stop();
	LOG("Scene imported in %ums.", timeMs);
	return true;
}