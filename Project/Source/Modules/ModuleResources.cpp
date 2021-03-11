#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/FileDialog.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceScene.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"
#include "FileSystem/JsonValue.h"
#include "FileSystem/SceneImporter.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/MaterialImporter.h"
#include "FileSystem/ShaderImporter.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleInput.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"
#include <string>
#include <future>
#include <chrono>

#include "Utils/Leaks.h"

bool ReadMetaFile(const char* filePath, rapidjson::Document& document) {
	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error reading meta file %s", filePath);
		return false;
	}

	// Parse document from file
	document.Parse<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return false;
	}

	return true;
}

void SaveMetaFile(const char* filePath, rapidjson::Document& document) {
	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	App->files->Save(filePath, stringBuffer.GetString(), stringBuffer.GetSize());
}

void ImportAsset(const char* filePath) {
	std::string extension = FileDialog::GetFileExtension(filePath);
	std::string metaFilePath = std::string(filePath) + META_EXTENSION;
	bool validMetaFile = App->files->Exists(metaFilePath.c_str());
	bool validResourceFiles = true;
	if (validMetaFile) {
		rapidjson::Document document;
		validMetaFile = ReadMetaFile(metaFilePath.c_str(), document);
		JsonValue jMeta(document, document);
		if (validMetaFile) {
			JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
			for (unsigned i = 0; i < jResourceIds.Size(); ++i) {
				UID id = jResourceIds[i];
				std::string resourcePath = App->resources->GenerateResourcePath(id);
				if (!App->files->Exists(resourcePath.c_str())) {
					validResourceFiles = false;
					break;
				}
			}
		}
	}

	if (!validMetaFile || !validResourceFiles) {
		bool validResourceFiles = true;
		rapidjson::Document document;
		JsonValue jMeta(document, document);
		UID id;
		if (App->files->Exists(metaFilePath.c_str())) {
			ReadMetaFile(metaFilePath.c_str(), document);
			id = jMeta[JSON_TAG_RESOURCE_IDS][0];
		} else {
			id = GenerateUID();
		}

		Resource* resource = nullptr;
		bool assetImported = true;
		if (extension == SCENE_EXTENSION) {
			// Scene files
			SceneImporter::ImportScene(filePath, jMeta);
		} else if (extension == MATERIAL_EXTENSION) {
			// Material files
			MaterialImporter::ImportMaterial(filePath, jMeta);
		} else if (extension == ".frag" || extension == ".vert" || extension == ".glsl") {
			// Shader files
			ShaderImporter::ImportShader(filePath, jMeta);
		} else if (extension == ".fbx" || extension == ".obj") {
			// Model files
			// ModelImporter::ImportModel(filePath, jMeta);
		} else if (extension == ".jpg" || extension == ".png" || extension == ".tif" || extension == ".dds" || extension == ".tga") {
			// Texture files
			TextureImporter::ImportTexture(filePath, jMeta);
		} else {
			assetImported = false;
		}

		if (!validMetaFile && assetImported) {
			jMeta[JSON_TAG_TIMESTAMP] = App->time->GetCurrentTimestamp();
			SaveMetaFile(metaFilePath.c_str(), document);
		}
	}
}

void CheckForNewAssetsRecursive(const char* path) {
	for (std::string& file : FileDialog::GetFilesInFolder(path)) {
		std::string filePath = std::string(path) + "/" + file;
		std::string extension = FileDialog::GetFileExtension(file.c_str());
		if (App->files->IsDirectory(filePath.c_str())) {
			CheckForNewAssetsRecursive(filePath.c_str());
		} else if (extension != META_EXTENSION) {
			ImportAsset(filePath.c_str());
		}
	}
}

bool ModuleResources::Init() {
	ilInit();
	iluInit();

	stopImportThread = false;
	importThread = std::thread(&ModuleResources::UpdateAsync, this);

	return true;
}

UpdateStatus ModuleResources::Update() {
	// Copy dropped file to assets folder
	const char* droppedFilePath = App->input->GetDroppedFilePath();
	if (droppedFilePath != nullptr) {
		std::string newFilePath = std::string(ASSETS_PATH) + "/" + FileDialog::GetFileNameAndExtension(droppedFilePath);
		FileDialog::Copy(droppedFilePath, newFilePath.c_str());
		App->input->ReleaseDroppedFilePath();
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleResources::CleanUp() {
	stopImportThread = true;
	importThread.join();

	for (std::pair<const UID, Resource*>& entry : resources) {
		RELEASE(entry.second);
	}

	return true;
}

Resource* ModuleResources::GetResourceByID(UID id) {
	resourcesMutex.lock();
	Resource* resource = resources.find(id) != resources.end() ? resources[id] : nullptr;
	resourcesMutex.unlock();
	return resource;
}

std::string ModuleResources::GenerateResourcePath(UID id) const {
	std::string strId = std::to_string(id);
	std::string metaFolder = DATA_PATH + strId.substr(0, 2);

	if (!App->files->Exists(metaFolder.c_str())) {
		App->files->CreateFolder(metaFolder.c_str());
	}

	return metaFolder + "/" + strId;
}

void ModuleResources::UpdateAsync() {
	while (!stopImportThread) {
		// Check if any asset file has been modified / deleted
		std::vector<UID> resourcesToRemove;
		std::vector<UID> resourcesToReimport;
		for (std::pair<const UID, Resource*>& entry : resources) {
			Resource* resource = entry.second;
			const std::string& resourceFilePath = resource->GetResourceFilePath();
			const std::string& assetFilePath = resource->GetAssetFilePath();
			std::string metaFilePath = assetFilePath + META_EXTENSION;

			// Check for deleted assets
			if (!App->files->Exists(assetFilePath.c_str())) {
				resourcesToRemove.push_back(entry.first);
				continue;
			}

			// Check for deleted, invalid or outdated meta files
			if (!App->files->Exists(metaFilePath.c_str())) {
				resourcesToRemove.push_back(entry.first);
				continue;
			} else {
				rapidjson::Document document;
				bool success = ReadMetaFile(metaFilePath.c_str(), document);
				JsonValue jMeta(document, document);
				if (success) {
					long long timestamp = jMeta[JSON_TAG_TIMESTAMP];
					if (App->files->GetLocalFileModificationTime(assetFilePath.c_str()) > timestamp) {
						// ASK: What happens when we update an asset?
						resourcesToRemove.push_back(entry.first);
						continue;
					}
				} else {
					resourcesToRemove.push_back(entry.first);
					continue;
				}
			}

			// Check for deleted resources
			if (!App->files->Exists(resourceFilePath.c_str())) {
				resourcesToReimport.push_back(entry.first);
			}
		}
		for (UID id : resourcesToReimport) {
			Resource* resource = resources[id];

			const std::string& resourceFilePath = resource->GetResourceFilePath();
			const std::string& assetFilePath = resource->GetAssetFilePath();

			if (!App->files->Exists(resourceFilePath.c_str())) {
				ImportAsset(assetFilePath.c_str());
			}
		}
		for (UID id : resourcesToRemove) {
			Resource* resource = resources[id];

			const std::string& assetFilePath = resource->GetAssetFilePath();
			const std::string& resourceFilePath = resource->GetResourceFilePath();
			std::string metaFilePath = assetFilePath + META_EXTENSION;
			if (App->files->Exists(metaFilePath.c_str())) {
				App->files->Erase(metaFilePath.c_str());
			}
			if (App->files->Exists(resourceFilePath.c_str())) {
				App->files->Erase(resourceFilePath.c_str());
			}

			resource->Unload();
			delete resource;
			resources.erase(id);
		}

		// Check if there are any new assets
		CheckForNewAssetsRecursive(ASSETS_PATH);

		std::this_thread::sleep_for(std::chrono::milliseconds(TIME_BETWEEN_RESOURCE_UPDATES_MS));
	}
}