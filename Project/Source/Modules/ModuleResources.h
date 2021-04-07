#pragma once

#include "Module.h"
#include "Utils/Pool.h"
#include "Utils/UID.h"
#include "Resources/Resource.h"
#include "Utils/AssetFile.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <concurrent_queue.h>

class ModuleResources : public Module {
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;
	void ReceiveEvent(const Event& ev) override;

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
	void SendAddResourceEvent(Resource* resource);

private:
	std::unordered_map<UID, Resource*> resources;
	std::unordered_map<UID, unsigned> referenceCounts;
	AssetFolder* rootFolder = nullptr;
	std::thread importThread;
	bool stopImportThread = false;
};

template<typename T>
inline T* ModuleResources::CreateResource(const char* assetFilePath, UID id) {
	std::string resourceFilePath = GenerateResourcePath(id);
	T* resource = new T(id, assetFilePath, resourceFilePath.c_str());
	SendAddResourceEvent(resource);
	return resource;
}
