#include "SceneImporter.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/MSTimer.h"
#include "Utils/FileDialog.h"
#include "FileSystem/TextureImporter.h"
#include "Resources/ResourceScene.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentMeshRenderer.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleScene.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include "Utils/Leaks.h"


#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"
#define JSON_TAG_ROOT "Root"
#define JSON_TAG_QUADTREE_BOUNDS "QuadtreeBounds"
#define JSON_TAG_QUADTREE_MAX_DEPTH "QuadtreeMaxDepth"
#define JSON_TAG_QUADTREE_ELEMENTS_PER_NODE "QuadtreeElementsPerNode"

bool SceneImporter::ImportScene(const char* filePath, JsonValue jMeta) {
	// Timer to measure importing a scene
	MSTimer timer;
	timer.Start();
	LOG("Importing scene from path: \"%s\".", filePath);

	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error loading scene %s", filePath);
		return false;
	}

	// Parse document from file
	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return false;
	}

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Create scene resource
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[0];
	UID id = jResource[JSON_TAG_ID];
	ResourceScene* scene = App->resources->CreateResource<ResourceScene>(filePath, id ? id : GenerateUID());

	// Add resource to meta file
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(scene->GetType());
	jResource[JSON_TAG_ID] = scene->GetId();

	// Save to file
	App->files->Save(scene->GetResourceFilePath().c_str(), stringBuffer.GetString(), stringBuffer.GetSize());

	unsigned timeMs = timer.Stop();
	LOG("Scene imported in %ums", timeMs);
	return true;
}

bool SceneImporter::SaveScene(const char* filePath) {
	// Create document
	rapidjson::Document document;
	document.SetObject();
	JsonValue jScene(document, document);

	// Save scene information
	Scene* scene = App->scene->scene;
	JsonValue jQuadtreeBounds = jScene[JSON_TAG_QUADTREE_BOUNDS];
	jQuadtreeBounds[0] = scene->quadtreeBounds.minPoint.x;
	jQuadtreeBounds[1] = scene->quadtreeBounds.minPoint.y;
	jQuadtreeBounds[2] = scene->quadtreeBounds.maxPoint.x;
	jQuadtreeBounds[3] = scene->quadtreeBounds.maxPoint.y;
	jScene[JSON_TAG_QUADTREE_MAX_DEPTH] = scene->quadtreeMaxDepth;
	jScene[JSON_TAG_QUADTREE_ELEMENTS_PER_NODE] = scene->quadtreeElementsPerNode;

	// Save GameObjects
	JsonValue jRoot = jScene[JSON_TAG_ROOT];
	scene->root->Save(jRoot);

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	App->files->Save(filePath, stringBuffer.GetString(), stringBuffer.GetSize());

	return true;
}

/*

static void ImportNode(const aiScene* assimpScene, const std::vector<Material>& materials, const aiNode* node, GameObject* parent, const float4x4& accumulatedTransform) {
	std::string name = node->mName.C_Str();
	LOG("Importing node: \"%s\"", name.c_str());

	if (name.find("$AssimpFbx$") != std::string::npos) { // Auxiliary node
		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			const float4x4& transform = accumulatedTransform * (*(float4x4*) &node->mTransformation);
			ImportNode(assimpScene, materials, node->mChildren[i], parent, transform);
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
		matrix.Decompose(position, rotation, scale);// TODO: this fails on non-uniform scale
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
			mesh->mesh = MeshImporter::ImportMesh(assimpMesh, i);
			mesh->mesh->materialIndex = i;

			// TODO: Move mesh loading to a better place
			MeshImporter::LoadMesh(mesh->mesh);

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
			ImportNode(assimpScene, materials, node->mChildren[i], gameObject, float4x4::identity);
		}
	}
}

bool SceneImporter::ImportScene(const char* filePath, GameObject* parent) {
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
				std::string modelFolderPath = FileDialog::GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				texture = TextureImporter::ImportTexture(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFile = FileDialog::GetFileNameAndExtension(materialFilePath.C_Str());
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
				std::string modelFolderPath = FileDialog::GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				texture = TextureImporter::ImportTexture(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFileName = FileDialog::GetFileName(materialFilePath.C_Str());
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
	ImportNode(assimpScene, materials, assimpScene->mRootNode, parent, float4x4::identity);

	unsigned timeMs = timer.Stop();
	LOG("Scene imported in %ums.", timeMs);
	return true;
}

bool SceneImporter::LoadScene(const char* fileName) {
	// Clear scene
	App->scene->ClearScene();
	App->editor->selectedGameObject = nullptr;

	// Timer to measure loading a scene
	MSTimer timer;
	timer.Start();

	// Read from file
	std::string filePath = std::string(SCENES_PATH) + "/" + fileName + SCENE_EXTENSION;
	Buffer<char> buffer = App->files->Load(filePath.c_str());

	if (buffer.Size() == 0) return false;

	// Parse document from file
	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return false;
	}
	JsonValue jScene(document, document);

	// Load GameObjects
	JsonValue jGameObjects = jScene[JSON_TAG_GAMEOBJECTS];
	unsigned jGameObjectsSize = jGameObjects.Size();
	Buffer<UID> ids(jGameObjectsSize);
	for (unsigned i = 0; i < jGameObjectsSize; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		GameObject* gameObject = App->scene->gameObjects.Obtain();
		gameObject->Load(jGameObject);

		UID id = gameObject->GetID();
		App->scene->gameObjectsIdMap[id] = gameObject;
		ids[i] = id;
	}

	// Post-load
	App->scene->root = App->scene->GetGameObject(jScene[JSON_TAG_ROOT_ID]);
	for (unsigned i = 0; i < jGameObjectsSize; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		UID id = ids[i];
		GameObject* gameObject = App->scene->GetGameObject(id);
		gameObject->PostLoad(jGameObject);
	}

	// Init components
	for (unsigned i = 0; i < jGameObjectsSize; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		UID id = ids[i];
		GameObject* gameObject = App->scene->GetGameObject(id);
		gameObject->InitComponents();
	}

	// Quadtree generation
	JsonValue jQuadtreeBounds = jScene[JSON_TAG_QUADTREE_BOUNDS];
	App->scene->quadtreeBounds = {{jQuadtreeBounds[0], jQuadtreeBounds[1]}, {jQuadtreeBounds[2], jQuadtreeBounds[3]}};
	App->scene->quadtreeMaxDepth = jScene[JSON_TAG_QUADTREE_MAX_DEPTH];
	App->scene->quadtreeElementsPerNode = jScene[JSON_TAG_QUADTREE_ELEMENTS_PER_NODE];
	App->scene->RebuildQuadtree();

	unsigned timeMs = timer.Stop();
	LOG("Scene loaded in %ums.", timeMs);
	return true;
}

bool SceneImporter::SaveScene(const char* fileName) {
	// Create document
	rapidjson::Document document;
	document.SetObject();
	JsonValue jScene(document, document);

	// Save scene information
	jScene[JSON_TAG_ROOT_ID] = App->scene->root->GetID();
	JsonValue jQuadtreeBounds = jScene[JSON_TAG_QUADTREE_BOUNDS];
	jQuadtreeBounds[0] = App->scene->quadtreeBounds.minPoint.x;
	jQuadtreeBounds[1] = App->scene->quadtreeBounds.minPoint.y;
	jQuadtreeBounds[2] = App->scene->quadtreeBounds.maxPoint.x;
	jQuadtreeBounds[3] = App->scene->quadtreeBounds.maxPoint.y;
	jScene[JSON_TAG_QUADTREE_MAX_DEPTH] = App->scene->quadtreeMaxDepth;
	jScene[JSON_TAG_QUADTREE_ELEMENTS_PER_NODE] = App->scene->quadtreeElementsPerNode;

	// Save GameObjects
	JsonValue jGameObjects = jScene[JSON_TAG_GAMEOBJECTS];
	unsigned i = 0;
	for (const GameObject& gameObject : App->scene->gameObjects) {
		JsonValue jGameObject = jGameObjects[i];

		GameObject* parent = gameObject.GetParent();
		jGameObject[JSON_TAG_PARENT_ID] = parent != nullptr ? parent->id : 0;

		gameObject.Save(jGameObject);

		i += 1;
	}

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	std::string filePath = std::string(SCENES_PATH) + "/" + fileName + SCENE_EXTENSION;
	App->files->Save(filePath.c_str(), stringBuffer.GetString(), stringBuffer.GetSize());

	return true;
}

*/
