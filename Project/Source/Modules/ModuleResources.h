#pragma once

#include "Module.h"
#include "Utils/Pool.h"
#include "Utils/UID.h"
#include "Resources/Resource.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <concurrent_queue.h>

struct AssetFile {
	AssetFile(const char* path);

	std::string path;
	std::vector<UID> resourceIds;
};

struct AssetFolder {
	AssetFolder(const char* path);

	std::string path;
	std::vector<AssetFolder> folders;
	std::vector<AssetFile> files;
};

enum class ResourceEventType {
	ADD_RESOURCE,
	REMOVE_RESOURCE,
	UPDATE_FOLDERS
};

struct ResourceEvent {
	ResourceEventType type = ResourceEventType::ADD_RESOURCE;
	void* object = nullptr;
};

class ModuleResources : public Module {
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	std::vector<UID> ImportAsset(const char* filePath);

	Resource* GetResource(UID id) const;
	AssetFolder* GetRootFolder() const;

	void IncreaseReferenceCount(UID id);
	void DecreaseReferenceCount(UID id);
	unsigned GetReferenceCount(UID id) const;

	std::string GenerateResourcePath(UID id) const;

	template<typename T>
	T* CreateResource(const char* assetFilePath, UID id);

private:
	void UpdateAsync();

	void CheckForNewAssetsRecursive(const char* path, AssetFolder* folder);

	Resource* CreateResourceByType(ResourceType type, const char* assetFilePath, UID id);

public:
	std::unordered_set<std::string> assetsToNotUpdate;

private:
	std::unordered_map<UID, Resource*> resources;
	std::unordered_map<UID, unsigned> referenceCounts;
	AssetFolder* rootFolder = nullptr;

	concurrency::concurrent_queue<ResourceEvent> resourceEventQueue;

	std::thread importThread;
	bool stopImportThread = false;
};

template<typename T>
inline T* ModuleResources::CreateResource(const char* assetFilePath, UID id) {
	std::string resourceFilePath = GenerateResourcePath(id);
	T* resource = new T(id, assetFilePath, resourceFilePath.c_str());
	resourceEventQueue.push({ResourceEventType::ADD_RESOURCE, resource});
	return resource;
}
