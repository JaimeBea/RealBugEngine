#include "ModelImporter.h"

#include "Application.h"
#include "Scene.h"
#include "GameObject.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/MSTimer.h"
#include "Utils/FileDialog.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentAnimation.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourcePrefab.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceClip.h"
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

#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"
#define JSON_TAG_ROOT "Root"

static ResourceMesh* ImportMesh(const char* modelFilePath, JsonValue jMeta, const aiMesh* assimpMesh, unsigned& resourceIndex, std::vector<const char*>& bones) {
	// Timer to measure importing a mesh
	MSTimer timer;
	timer.Start();

	unsigned numVertices = assimpMesh->mNumVertices;
	unsigned numIndices = assimpMesh->mNumFaces * 3;
	unsigned materialIndex = assimpMesh->mMaterialIndex;
	unsigned numBones = assimpMesh->mNumBones;

	// Save to custom format buffer
	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned bonesIDSize = sizeof(unsigned) * 4;
	unsigned weightsSize = sizeof(float) * 4;
	unsigned indexSize = sizeof(unsigned);
	unsigned boneSize = sizeof(unsigned) + sizeof(char) * FILENAME_MAX + sizeof(float) * 10;

	unsigned headerSize = sizeof(unsigned) * 3;
	unsigned vertexSize = positionSize + normalSize + uvSize + bonesIDSize + weightsSize;
	unsigned vertexBufferSize = vertexSize * numVertices;
	unsigned indexBufferSize = indexSize * numIndices;
	unsigned bonesBufferSize = boneSize * numBones;

	size_t size = headerSize + bonesBufferSize + vertexBufferSize + indexBufferSize;
	Buffer<char> buffer = Buffer<char>(size);
	char* cursor = buffer.Data();

	*((unsigned*) cursor) = numVertices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = numIndices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = numBones;
	cursor += sizeof(unsigned);

	std::vector<ResourceMesh::Attach> attaches;
	attaches.resize(numVertices);
	for (unsigned i = 0; i < assimpMesh->mNumBones; ++i) {
		aiBone* aiBone = assimpMesh->mBones[i];

		bones.push_back(aiBone->mName.C_Str());

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
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[resourceIndex];
	UID id = jResource[JSON_TAG_ID];
	ResourceMesh* mesh = App->resources->CreateResource<ResourceMesh>(modelFilePath, id ? id : GenerateUID());

	// Add resource to meta file
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(mesh->GetType());
	jResource[JSON_TAG_ID] = mesh->GetId();
	resourceIndex += 1;

	// Save buffer to file
	const std::string& resourceFilePath = mesh->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save meshRenderer resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("Mesh imported in %ums", timeMs);
	return mesh;
}

static ResourceAnimation* ImportAnimation(const char* modelFilePath, JsonValue jMeta, const aiAnimation* aiAnim, const aiScene* assimpScene, unsigned& resourceIndex) {
	// Timer to measure importing an animation
	MSTimer timer;
	timer.Start();

	unsigned headerSize = sizeof(unsigned) * 2 + sizeof(float);

	unsigned numKeyFrames = std::max(aiAnim->mChannels[0]->mNumPositionKeys, aiAnim->mChannels[0]->mNumRotationKeys);
	unsigned numChannels = aiAnim->mNumChannels;

	unsigned keyFrameVSize = sizeof(ResourceAnimation::Channel);
	unsigned keyFrameKSize = FILENAME_MAX / 2 * sizeof(char);
	unsigned keyFrameKLengthSize = sizeof(unsigned);
	unsigned keyFrameTotalSize = keyFrameVSize + keyFrameKSize + keyFrameKLengthSize;
	unsigned bufferSize = headerSize + numKeyFrames * numChannels * keyFrameTotalSize;

	Buffer<char> buffer(bufferSize);
	char* cursor = buffer.Data();

	float ticks = static_cast<float>(aiAnim->mTicksPerSecond);
	float duration = static_cast<float>(aiAnim->mDuration);
	float durationInSeconds = ticks > 0 ? (duration / ticks) : 0;
	*((float*) cursor) = durationInSeconds;
	cursor += sizeof(float);

	std::vector<ResourceAnimation::KeyFrameChannels> keyFrames;
	keyFrames.resize(numKeyFrames);

	aiNode* rootNode = assimpScene->mRootNode;

	for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
		aiNodeAnim* aiChannel = aiAnim->mChannels[i];
		std::string channelName(aiChannel->mNodeName.C_Str());

		aiNode* parent = rootNode->FindNode(aiChannel->mNodeName.C_Str())->mParent;

		size_t pos = channelName.find("_$AssimpFbx$");
		if (pos != std::string::npos) {
			channelName = channelName.substr(0, pos);
		}

		float4x4 accumulatedTransform = float4x4::identity;
		bool assimpNode = (std::string(parent->mName.C_Str()).find("$AssimpFbx$") != std::string::npos);

		while (assimpNode) {
			aiVector3D pos, scale;
			aiQuaternion rot;
			(parent->mTransformation).Decompose(scale, rot, pos);
			accumulatedTransform = float4x4::FromTRS(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w), float3(scale.x, scale.y, scale.z)) * accumulatedTransform;
			parent = parent->mParent;
			assimpNode = (std::string(parent->mName.C_Str()).find("$AssimpFbx$") != std::string::npos);
		}

		unsigned int frame = 0;

		for (unsigned int j = 0; j < numKeyFrames; ++j) {
			aiQuaternion aiQuat = (aiChannel->mNumRotationKeys > 1) ? aiChannel->mRotationKeys[j].mValue : aiChannel->mRotationKeys[0].mValue;
			aiVector3D aiV3D = (aiChannel->mNumPositionKeys > 1) ? aiChannel->mPositionKeys[j].mValue : aiChannel->mPositionKeys[0].mValue;

			float3 accumulatedPosition, accumulatedScaling;
			Quat accumulatedRotation;
			accumulatedTransform.Decompose(accumulatedPosition, accumulatedRotation, accumulatedScaling);

			ResourceAnimation::Channel channel = keyFrames[frame].channels[channelName];
			channel.rotation = channel.rotation * accumulatedRotation * Quat(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
			channel.tranlation = channel.tranlation + accumulatedPosition + float3(aiV3D.x, aiV3D.y, aiV3D.z);

			keyFrames[frame++].channels[channelName] = channel;
		}
	}

	*((unsigned*) cursor) = keyFrames.size();
	cursor += sizeof(unsigned);

	*((unsigned*) cursor) = keyFrames[0].channels.size();
	cursor += sizeof(unsigned);

	for (const ResourceAnimation::KeyFrameChannels& keyFrame : keyFrames) {
		for (const auto& entry : keyFrame.channels) {
			const std::string& channelName = entry.first;
			const ResourceAnimation::Channel& channel = entry.second;

			*((unsigned*) cursor) = channelName.length();
			cursor += sizeof(unsigned);

			memcpy_s(cursor, channelName.size() * sizeof(char), channelName.data(), channelName.size() * sizeof(char));
			cursor += (FILENAME_MAX / 2) * sizeof(char);

			// Translation
			*((float*) cursor) = channel.tranlation.x;
			cursor += sizeof(float);
			*((float*) cursor) = channel.tranlation.y;
			cursor += sizeof(float);
			*((float*) cursor) = channel.tranlation.z;
			cursor += sizeof(float);

			// Rotation
			*((float*) cursor) = channel.rotation.x;
			cursor += sizeof(float);
			*((float*) cursor) = channel.rotation.y;
			cursor += sizeof(float);
			*((float*) cursor) = channel.rotation.z;
			cursor += sizeof(float);
			*((float*) cursor) = channel.rotation.w;
			cursor += sizeof(float);
		}
	}

	// Create animation
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[resourceIndex];
	UID id = jResource[JSON_TAG_ID];
	ResourceAnimation* animation = App->resources->CreateResource<ResourceAnimation>(modelFilePath, id ? id : GenerateUID());
	animation->keyFrames = keyFrames;
	animation->duration = duration;

	// Add resource to meta file
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(animation->GetType());
	jResource[JSON_TAG_ID] = animation->GetId();
	resourceIndex += 1;

	// Save buffer to file
	const std::string& resourceFilePath = animation->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save animation resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("Animation imported in %ums", timeMs);
	return animation;
}

static void ImportNode(const char* modelFilePath, JsonValue jMeta, const aiScene* assimpScene, const aiNode* node, Scene* scene, GameObject* parent, std::vector<UID>& materialIds, const float4x4& accumulatedTransform, unsigned& resourceIndex, std::vector<const char*>& bones) {
	std::string name = node->mName.C_Str();
	LOG("Importing node: \"%s\"", name.c_str());

	if (name.find("$AssimpFbx$") != std::string::npos) { // Auxiliary node
		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			const float4x4& transform = accumulatedTransform * (*(float4x4*) &node->mTransformation);
			ImportNode(modelFilePath, jMeta, assimpScene, node->mChildren[i], scene, parent, materialIds, transform, resourceIndex, bones);
		}
	} else { // Normal node
		// Create GameObject
		GameObject* gameObject = scene->CreateGameObject(parent, GenerateUID(), name.c_str());

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
		LOG("Transform: (%f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f)", position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, rotation.w, scale.x, scale.y, scale.z);

		// Save min and max points
		vec minPoint = vec(FLOAT_INF, FLOAT_INF, FLOAT_INF);
		vec maxPoint = vec(-FLOAT_INF, -FLOAT_INF, -FLOAT_INF);

		// Load meshes
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			LOG("Importing meshRenderer %i", i);
			aiMesh* assimpMesh = assimpScene->mMeshes[node->mMeshes[i]];

			ComponentMeshRenderer* meshRenderer = gameObject->CreateComponent<ComponentMeshRenderer>();
			meshRenderer->meshId = ImportMesh(modelFilePath, jMeta, assimpMesh, resourceIndex, bones)->GetId();
			meshRenderer->materialId = materialIds[assimpMesh->mMaterialIndex];

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
		}

		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			ImportNode(modelFilePath, jMeta, assimpScene, node->mChildren[i], scene, gameObject, materialIds, float4x4::identity, resourceIndex, bones);
		}
	}
}

bool ModelImporter::ImportModel(const char* filePath, JsonValue jMeta) {
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

	// Initialize resource accumulator
	unsigned resourceIndex = 0;
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];

	// Import materials
	std::vector<UID> materialIds;
	for (unsigned i = 0; i < assimpScene->mNumMaterials; ++i) {
		LOG("Loading material %i...", i);
		aiMaterial* assimpMaterial = assimpScene->mMaterials[i];

		JsonValue jResource = jResources[resourceIndex];
		UID id = jResource[JSON_TAG_ID];
		ResourceMaterial* material = App->resources->CreateResource<ResourceMaterial>(filePath, id ? id : GenerateUID());

		aiString materialFilePath;
		aiTextureMapping mapping;
		aiColor4D color;
		unsigned uvIndex;

		std::vector<UID>& shaderResourceIds = App->resources->ImportAsset(SHADERS_PATH "/" PHONG_SHADER_FILE);
		if (shaderResourceIds.empty()) {
			LOG("Unable to find phong shader file.");
		} else {
			material->shaderId = shaderResourceIds[0];
		}

		if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &materialFilePath, &mapping, &uvIndex) == AI_SUCCESS) {
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uvIndex == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import diffuse texture...");
			std::vector<UID>& textureResourceIds = App->resources->ImportAsset(materialFilePath.C_Str());

			// Try to load relative to the model folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to model folder...");
				std::string modelFolderPath = FileDialog::GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				textureResourceIds = App->resources->ImportAsset(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFile = FileDialog::GetFileNameAndExtension(materialFilePath.C_Str());
				std::string texturesFolderMaterialFileDir = std::string(TEXTURES_PATH) + "/" + materialFile;
				textureResourceIds = App->resources->ImportAsset(texturesFolderMaterialFileDir.c_str());
			}

			if (textureResourceIds.empty()) {
				LOG("Unable to find diffuse texture file.");
			} else {
				LOG("Diffuse texture imported successfuly.");
				material->diffuseMapId = textureResourceIds[0];
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
			std::vector<UID>& textureResourceIds = App->resources->ImportAsset(materialFilePath.C_Str());

			// Try to load relative to the model folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to model folder...");
				std::string modelFolderPath = FileDialog::GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				textureResourceIds = App->resources->ImportAsset(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFileName = FileDialog::GetFileName(materialFilePath.C_Str());
				std::string texturesFolderMaterialFileDir = std::string(TEXTURES_PATH) + "/" + materialFileName + TEXTURE_EXTENSION;
				textureResourceIds = App->resources->ImportAsset(texturesFolderMaterialFileDir.c_str());
			}

			if (textureResourceIds.empty()) {
				LOG("Unable to find specular texture file.");
			} else {
				LOG("Specular texture imported successfuly.");
				material->specularMapId = textureResourceIds[0];
			}
		} else {
			LOG("Specular texture not found.");
		}

		if (assimpMaterial->GetTexture(aiTextureType_METALNESS, 0, &materialFilePath, &mapping, &uvIndex) == AI_SUCCESS) {
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uvIndex == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import metalness texture...");
			std::vector<UID>& textureResourceIds = App->resources->ImportAsset(materialFilePath.C_Str());

			// Try to load relative to the model folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to model folder...");
				std::string modelFolderPath = FileDialog::GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				textureResourceIds = App->resources->ImportAsset(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFileName = FileDialog::GetFileName(materialFilePath.C_Str());
				std::string texturesFolderMaterialFileDir = std::string(TEXTURES_PATH) + "/" + materialFileName + TEXTURE_EXTENSION;
				textureResourceIds = App->resources->ImportAsset(texturesFolderMaterialFileDir.c_str());
			}

			if (textureResourceIds.empty()) {
				LOG("Unable to find metalness texture file.");
			} else {
				LOG("Metalness texture imported successfuly.");
				material->metallicMapId = textureResourceIds[0];
			}
		} else {
			LOG("Metalness texture not found.");
		}

		if (assimpMaterial->GetTexture(aiTextureType_NORMALS, 0, &materialFilePath, &mapping, &uvIndex) == AI_SUCCESS) {
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uvIndex == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import normals texture...");
			std::vector<UID>& textureResourceIds = App->resources->ImportAsset(materialFilePath.C_Str());

			// Try to load relative to the model folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to model folder...");
				std::string modelFolderPath = FileDialog::GetFileFolder(filePath);
				std::string modelFolderMaterialFilePath = modelFolderPath + "/" + materialFilePath.C_Str();
				textureResourceIds = App->resources->ImportAsset(modelFolderMaterialFilePath.c_str());
			}

			// Try to load relative to the textures folder
			if (textureResourceIds.empty()) {
				LOG("Trying to import texture relative to textures folder...");
				std::string materialFileName = FileDialog::GetFileName(materialFilePath.C_Str());
				std::string texturesFolderMaterialFileDir = std::string(TEXTURES_PATH) + "/" + materialFileName + TEXTURE_EXTENSION;
				textureResourceIds = App->resources->ImportAsset(texturesFolderMaterialFileDir.c_str());
			}

			if (textureResourceIds.empty()) {
				LOG("Unable to find normals texture file.");
			} else {
				LOG("Normals texture imported successfuly.");
				material->normalMapId = textureResourceIds[0];
			}
		} else {
			LOG("Normals texture not found.");
		}

		assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, material->diffuseColor);
		assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, material->specularColor);
		assimpMaterial->Get(AI_MATKEY_SHININESS, material->smoothness);

		// Add resource to meta file
		jResource[JSON_TAG_TYPE] = GetResourceTypeName(material->GetType());
		jResource[JSON_TAG_ID] = material->GetId();
		resourceIndex += 1;

		material->SaveToFile(material->GetResourceFilePath().c_str());
		materialIds.push_back(material->GetId());
		LOG("Material imported.");
	}

	// Import nodes
	Scene scene(1000);
	LOG("Importing scene tree.");
	GameObject* root = scene.CreateGameObject(nullptr, GenerateUID(), "Root");
	root->CreateComponent<ComponentTransform>();

	std::vector<const char*> bones;
	ImportNode(filePath, jMeta, assimpScene, assimpScene->mRootNode, &scene, root, materialIds, float4x4::identity, resourceIndex, bones);

	// Load animations
	if (assimpScene->mNumAnimations > 0) {
		LOG("Importing animations");

		//JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
		JsonValue jResource = jResources[resourceIndex];
		UID id = jResource[JSON_TAG_ID];
		ResourceStateMachine* resourceStateMachine = App->resources->CreateResource<ResourceStateMachine>(filePath, id ? id : GenerateUID());
		UID uidSM = resourceStateMachine->GetId();

		jResource[JSON_TAG_TYPE] = GetResourceTypeName(resourceStateMachine->GetType());
		jResource[JSON_TAG_ID] = resourceStateMachine->GetId();
		resourceIndex += 1;

		std::string resourceName = "animation";
		std::string stateName = "state";
		std::string clipName = "clip";
			
		//ResourceAnimation* testAnim = nullptr; //TODO::Delete this

		States initialState;
		for (unsigned int i = 0; i < assimpScene->mNumAnimations; ++i) {
			std::string parsedI = std::to_string(i);

			ResourceAnimation* animation = ImportAnimation(filePath, jMeta, assimpScene->mAnimations[i], assimpScene, resourceIndex);

			/*if (testAnim == nullptr) { //TODO::Delete this
				testAnim = animation;
			}*/

			resourceStateMachine->resourceAnimations.insert(std::make_pair(resourceName + parsedI, animation));

			JsonValue jResourceClip = jResources[resourceIndex];
			UID idClip = jResourceClip[JSON_TAG_ID];
			ResourceClip clip = *App->resources->CreateResource<ResourceClip>(filePath, idClip ? idClip : GenerateUID());
			clip.Init(clipName + parsedI, animation->GetId(), 0, animation->keyFrames.size() - 1, true);
			//clip.SetSpeed(2.0);

			jResourceClip[JSON_TAG_TYPE] = GetResourceTypeName(clip.GetType());
			jResourceClip[JSON_TAG_ID] = clip.GetId();
			resourceIndex += 1;
			clip.SaveToFile(clip.GetResourceFilePath().c_str());

			//Adding initial state
			initialState = resourceStateMachine->AddState(stateName + parsedI, clip.GetId());
		}


		/* TODO:: Hardcoded for testing animation
		//Setting machine state
		//Dead animation
		std::string sState2 = "State2";
		std::string clipName2 = "testClip2";
		JsonValue jResourceClip2 = jResources[resourceIndex];
		UID idClip2 = jResourceClip2[JSON_TAG_ID];
		ResourceClip clip2 = *App->resources->CreateResource<ResourceClip>(filePath, idClip2 ? idClip2 : GenerateUID());
		clip2.Init(clipName2, testAnim->GetId(), 290, 360, false);
		jResourceClip2[JSON_TAG_TYPE] = GetResourceTypeName(clip2.GetType());
		jResourceClip2[JSON_TAG_ID] = clip2.GetId();
		resourceIndex += 1;
		clip2.SaveToFile(clip2.GetResourceFilePath().c_str());

		//idle animation
		std::string sState3 = "State3";
		std::string clipName3 = "testClip3";
		JsonValue jResourceClip3 = jResources[resourceIndex];
		UID idClip3 = jResourceClip3[JSON_TAG_ID];
		ResourceClip clip3 = *App->resources->CreateResource<ResourceClip>(filePath, idClip3 ? idClip3 : GenerateUID());
		clip3.Init(clipName3, testAnim->GetId(), 60, 120,  true );
		jResourceClip3[JSON_TAG_TYPE] = GetResourceTypeName(clip3.GetType());
		jResourceClip3[JSON_TAG_ID] = clip3.GetId();
		resourceIndex += 1;
		clip3.SaveToFile(clip3.GetResourceFilePath().c_str());

		//Mocking transition
		States state2 = resourceStateMachine->AddState(sState2, clip2.GetId());
		States state3 = resourceStateMachine->AddState(sState3, clip3.GetId());

		std::string tName1 = "s1Ts2";
		std::string tName2 = "s2Ts1";
		std::string tName3 = "s2Ts3";
		std::string tName4 = "s3Ts1";

		resourceStateMachine->AddTransition(initialState, state2, 0.3, tName1);
		resourceStateMachine->AddTransition(state2, initialState, 0.3, tName2);
		resourceStateMachine->AddTransition(state2, state3, 0.4, tName3);
		resourceStateMachine->AddTransition(state3, initialState, 0.7, tName4);*/

		ComponentAnimation* animationComponent = root->GetChildren()[0]->CreateComponent<ComponentAnimation>();
		animationComponent->stateMachineResourceUID = resourceStateMachine->GetId();
		animationComponent->SetInitialState(initialState);

		resourceStateMachine->SaveToFile(resourceStateMachine->GetResourceFilePath().c_str());
		bool ok = true;
	}

	// Cache bones for skinning
	aiNode* rootBone = nullptr;

	if (!bones.empty()) {
		rootBone = assimpScene->mRootNode->FindNode(bones[0]);
		aiNode* rootBoneParent = rootBone->mParent;
		bool foundInBones = false;
		do {
			// Ignore assimp middle nodes
			std::string name = rootBoneParent->mName.C_Str();
			while (name.find("$AssimpFbx$") != std::string::npos) {
				rootBoneParent = rootBoneParent->mParent;
				name = rootBoneParent->mName.C_Str();
			}
			// Find if node in bones
			foundInBones = false;
			for (const char* bone : bones) {
				if (std::strcmp(rootBoneParent->mName.C_Str(), bone) != 0) continue;

				foundInBones = true;
				rootBone = rootBoneParent;
				rootBoneParent = rootBone->mParent;
			}
		} while (foundInBones);
	}

	if (!bones.empty()) {
		GameObject* rootBoneGO = root->FindDescendant(rootBone->mName.C_Str());
		root->GetChildren()[0]->SetRootBone(rootBoneGO);

		std::unordered_map<std::string, GameObject*> goBones;
		// TODO: check if CtrlGrp is generated always
		goBones[rootBoneGO->name] = rootBoneGO;
		CacheBones(rootBoneGO, goBones);

		for (GameObject* child : root->GetChildren()[0]->GetChildren()) {
			if (child->name != rootBoneGO->name) {
				SaveBones(child, goBones);
			}
		}
	}

	// Save prefab
	SavePrefab(filePath, jMeta, root->GetChildren()[0], resourceIndex);

	// Delete temporary GameObject
	scene.DestroyGameObject(root);

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
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[resourceIndex];
	UID id = jResource[JSON_TAG_ID];
	ResourcePrefab* prefabResource = App->resources->CreateResource<ResourcePrefab>(filePath, id ? id : GenerateUID());
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(prefabResource->GetType());
	jResource[JSON_TAG_ID] = prefabResource->GetId();
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

void ModelImporter::CacheBones(GameObject* node, std::unordered_map<std::string, GameObject*>& goBones) {
	for (GameObject* child : node->GetChildren()) {
		goBones[child->name] = child;
		CacheBones(child, goBones);
	}
}

void ModelImporter::SaveBones(GameObject* node, std::unordered_map<std::string, GameObject*>& goBones) {
	for (ComponentMeshRenderer& meshRenderer : node->GetComponents<ComponentMeshRenderer>()) {
		meshRenderer.goBones = goBones;
	}

	for (GameObject* child : node->GetChildren()) {
		SaveBones(child, goBones);
	}
}