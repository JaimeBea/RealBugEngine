#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceScene.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"
#include "FileSystem/JsonValue.h"
#include "FileSystem/MeshImporter.h"
#include "FileSystem/TextureImporter.h"
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

#include "Utils/Leaks.h"

bool ReadMetaFile(const char* filePath, rapidjson::Document& document) {
	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error reading meta file %s", filePath);
		return false;
	}

	// Parse document from file
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
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

bool ModuleResources::Init() {
	ilInit();
	iluInit();

	return true;
}

UpdateStatus ModuleResources::Update() {
	// Copy dropped file to assets folder
	const char* droppedFilePath = App->input->GetDroppedFilePath();
	bool fileCopied = false;
	if (droppedFilePath != nullptr) {
		std::string newFilePath = std::string(ASSETS_PATH) + "/" + App->files->GetFileNameAndExtension(droppedFilePath);
		App->files->Copy(droppedFilePath, newFilePath.c_str());
		App->input->ReleaseDroppedFilePath();
		fileCopied = true;
	}

	timeAccumulator += App->time->GetRealTimeDeltaTime();
	if (fileCopied || timeAccumulator >= timeBetweenUpdates) {
		// Check if any resource file has been modified / deleted
		std::vector<UID> resourcesToRemove;
		for (std::pair<const UID, Resource*>& entry : resources) {
			Resource* resource = entry.second;
			std::string filePath = resource->GetLocalFilePath();
			std::string metaFilePath = filePath + META_EXTENSION;

			// Remove old files
			if (!App->files->Exists(filePath.c_str())) {
				resourcesToRemove.push_back(entry.first);
				continue;
			}

			// Check for modified files
			if (App->files->Exists(metaFilePath.c_str())) {
				rapidjson::Document document;
				bool success = ReadMetaFile(metaFilePath.c_str(), document);
				JsonValue jMeta(document, document);
				if (success) {
					long long timestamp = jMeta["Timestamp"];
					if (App->files->GetLocalFileModificationTime(filePath.c_str()) > timestamp) {
						resource->Import();
					}
				} else {
					resource->Import();
				}
			}
		}
		for (UID id : resourcesToRemove) {
			Resource* resource = resources[id];
			std::string filePath = resource->GetLocalFilePath();
			std::string metaFilePath = filePath + META_EXTENSION;
			App->files->Erase(metaFilePath.c_str());
			resource->Delete();
			delete resource;
			resources.erase(id);
		}

		// Check if any resource files have been added
		CheckForNewFilesRecursive(ASSETS_PATH);

		timeAccumulator = 0.0f;
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleResources::CleanUp() {
	for (std::pair<const UID, Resource*>& entry : resources) {
		RELEASE(entry.second);
	}

	return true;
}

Resource* ModuleResources::GetResourceByID(UID id) {
	if (resources.find(id) == resources.end()) {
		return nullptr;
	}
	return resources[id];
}

void ModuleResources::CheckForNewFilesRecursive(const char* path) {
	for (std::string& file : App->files->GetFilesInLocalFolder(path)) {
		std::string filePath = std::string(path) + "/" + file;
		if (App->files->IsDirectory(file.c_str())) {
			CheckForNewFilesRecursive(filePath.c_str());
		} else {
			// Check if the file is not in resource_files
			if (resource_files.find(filePath.c_str()) == resource_files.end()) {
				std::string metaFilePath = filePath + META_EXTENSION;
				bool validMetaFile = App->files->Exists(metaFilePath.c_str());
				if (validMetaFile) {
					rapidjson::Document document;
					validMetaFile = ReadMetaFile(metaFilePath.c_str(), document);
					JsonValue jMeta(document, document);
					unsigned long long id = jMeta["ID"];
					if (validMetaFile) {
						Resource* resource = CreateNewResource(filePath.c_str());
						if (resource != nullptr) {
							resource->LoadImportSettings(jMeta);
							resource->Import();
						}
					}
				}

				if (!validMetaFile) {
					rapidjson::Document document;
					JsonValue jMeta(document, document);
					Resource* resource = CreateNewResource(filePath.c_str());
					if (resource != nullptr) {
						resource->Import();
						resource->SaveImportSettings(jMeta);
						SaveMetaFile(metaFilePath.c_str(), document);
					}
				}
			}
		}
	}
}

Resource* ModuleResources::CreateNewResource(const char* path) {
	std::string extension = App->files->GetFileExtension(path);

	Resource* resource = nullptr;
	UID id = GenerateUID();
	if (extension == SCENE_EXTENSION) {
		// Scene files
	} else if (extension == ".fbx" || extension == ".obj") {
		// Model files
		// TODO: Make multiple resources being created possible
	} else if (extension == ".jpg" || extension == ".png" || extension == ".tif" || extension == ".dds" || extension == ".tga") {
		// Texture files
		resource = new ResourceTexture(id, path);
		resources.emplace(id, resource);
	}

	return resource;
}
