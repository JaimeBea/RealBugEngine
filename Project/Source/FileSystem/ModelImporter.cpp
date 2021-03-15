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
#include "Resources/ResourceTexture.h"
#include "Resources/ResourcePrefab.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#define JSON_TAG_ROOT "Root"

ResourceMesh* ImportMesh(const char* modelFilePath, JsonValue jMeta, const aiMesh* assimpMesh, unsigned& resourceIndex) {
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
		LOG("Failed to save meshRenderer resource.");
		return false;
	}

	resourceIndex += 1;
	unsigned timeMs = timer.Stop();
	LOG("Mesh imported in %ums", timeMs);
	return mesh;
}

void ImportNode(const char* modelFilePath, JsonValue jMeta, const aiScene* assimpScene, const aiNode* node, GameObject* parent, const float4x4& accumulatedTransform, unsigned& resourceIndex) {
	std::string name = node->mName.C_Str();
	LOG("Importing node: \"%s\"", name.c_str());

	if (name.find("$AssimpFbx$") != std::string::npos) { // Auxiliary node
		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			const float4x4& transform = accumulatedTransform * (*(float4x4*) &node->mTransformation);
			ImportNode(modelFilePath, jMeta, assimpScene, node->mChildren[i], parent, transform, resourceIndex);
		}
	} else { // Normal node
		// Create GameObject
		GameObject* gameObject = App->scene->CreateGameObject(parent, GenerateUID(), name.c_str());

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
			LOG("Importing meshRenderer %i", i);
			aiMesh* assimpMesh = assimpScene->mMeshes[node->mMeshes[i]];

			ComponentMeshRenderer* meshRenderer = gameObject->CreateComponent<ComponentMeshRenderer>();
			meshRenderer->mesh = ImportMesh(modelFilePath, jMeta, assimpMesh, resourceIndex);

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
			ImportNode(modelFilePath, jMeta, assimpScene, node->mChildren[i], gameObject, float4x4::identity, resourceIndex);
		}
	}
}

bool ModelImporter::ImportModel(const char* filePath, JsonValue jMeta) {
	// TODO: Load materials

	// Timer to measure importing a model
	MSTimer timer;
	timer.Start();

	// Check for extension support
	std::string extension = FileDialog::GetFileExtension(filePath);
	if (!aiIsExtensionSupported(extension.c_str())) {
		LOG("Extension is not supported by assimp: \"%s\".", extension);
		return false;
	}

	// Import model
	LOG("Importing scene from path: \"%s\".", filePath);
	const aiScene* assimpScene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
	DEFER {
		aiReleaseImport(assimpScene);
	};
	if (!assimpScene) {
		LOG("Error importing scene: %s", filePath, aiGetErrorString());
		return false;
	}

	// Import nodes
	LOG("Importing scene tree.");
	unsigned resourceIndex = 0;
	GameObject* root = App->scene->CreateGameObject(nullptr, GenerateUID(), "Root");
	root->CreateComponent<ComponentTransform>();
	ImportNode(filePath, jMeta, assimpScene, assimpScene->mRootNode, root, float4x4::identity, resourceIndex);

	// Save prefab
	SavePrefab(filePath, jMeta, root->GetChildren()[0], resourceIndex);

	// Delete temporary GameObject
	App->scene->DestroyGameObject(root);

	unsigned timeMs = timer.Stop();
	LOG("Scene imported in %ums.", timeMs);
	return true;
}

bool ModelImporter::SavePrefab(const char* filePath, JsonValue jMeta, GameObject* root, unsigned& resourceIndex) {
	// Create document
	rapidjson::Document document;
	document.SetObject();
	JsonValue jScene(document, document);

	// Save GameObjects
	JsonValue jRoot = jScene[JSON_TAG_ROOT];
	root->SavePrototype(jRoot);

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Create prefab resource
	ResourcePrefab* prefabResource = App->resources->CreateResource<ResourcePrefab>(filePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[resourceIndex] = prefabResource->GetId();
	resourceIndex += 1;

	// Save to file
	const std::string& resourceFilePath = prefabResource->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), stringBuffer.GetString(), stringBuffer.GetSize());
	if (!saved) {
		LOG("Failed to save prefab resource.");
		return false;
	}

	return true;
}