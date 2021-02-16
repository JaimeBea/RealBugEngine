#pragma once

#include "Module.h"
#include "Utils/Pool.h"
#include "Utils/UID.h"
#include "Resources/Resource.h"

#include <unordered_set>
#include <unordered_map>

#define JSON_TAG_RESOURCE_IDS "ResourceIds"
#define JSON_TAG_TIMESTAMP "Timestamp"

class ModuleResources : public Module {
public:
	bool Init() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	Resource* GetResourceByID(UID id);
	std::string GenerateResourcePath(UID id) const;

	template<typename T>
	T* CreateResource(const char* assetFilePath);

public:
	float timeBetweenUpdates = 1.0f;

private:
	void CheckForNewAssetsRecursive(const char* path);
	void ImportAsset(const char* filePath);

private:
	std::unordered_map<UID, Resource*> resources;
	float timeAccumulator = 0.0f;
};

template<typename T>
inline T* ModuleResources::CreateResource(const char* assetFilePath) {
	UID id = GenerateUID();
	std::string resourceFilePath = GenerateResourcePath(id);
	T* resource = new T(id, assetFilePath, resourceFilePath.c_str());
	resources.emplace(id, resource);
	return resource;
}
