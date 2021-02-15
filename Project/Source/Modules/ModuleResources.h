#pragma once

#include "Module.h"
#include "Utils/Pool.h"
#include "Utils/UID.h"
#include "Resources/Resource.h"

#include <unordered_set>
#include <unordered_map>

class ModuleResources : public Module {
public:
	bool Init() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	Resource* GetResourceByID(UID id);

public:
	float timeBetweenUpdates = 1.0f;

private:
	void CheckForNewFilesRecursive(const char* path);
	Resource* CreateNewResource(const char* path);

private:
	std::unordered_map<UID, Resource*> resources;

	std::unordered_set<std::string> resource_files;

	float timeAccumulator = 0.0f;
};
