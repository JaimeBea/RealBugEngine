#pragma once

#include "Module.h"
#include "Utils/Pool.h"
#include "Utils/UID.h"
#include "Resources/Resource.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>

#define JSON_TAG_RESOURCE_IDS "ResourceIds"
#define JSON_TAG_TIMESTAMP "Timestamp"

class ModuleResources : public Module {
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	Resource* GetResourceByID(UID id);
	std::string GenerateResourcePath(UID id) const;

	std::vector<Resource*> ImportAsset(const char* filePath);

	template<typename T>
	T* CreateResource(const char* assetFilePath);

private:
	void UpdateAsync();

	void CheckForNewAssetsRecursive(const char* path);

private:
	std::unordered_map<UID, Resource*> resources;
	std::mutex resourcesMutex;

	std::thread importThread;
	bool stopImportThread = false;
};

template<typename T>
inline T* ModuleResources::CreateResource(const char* assetFilePath) {
	UID id = GenerateUID();
	std::string resourceFilePath = GenerateResourcePath(id);
	T* resource = new T(id, assetFilePath, resourceFilePath.c_str());
	resourcesMutex.lock();
	resources.emplace(id, resource);
	resourcesMutex.unlock();
	return resource;
}
