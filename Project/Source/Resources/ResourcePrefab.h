#pragma once

#include "Resources/Resource.h"

class GameObject;

class ResourcePrefab : public Resource {
public:
	ResourcePrefab(UID id, const char* assetFilePath, const char* resourceFilePath);

	void BuildPrefab(GameObject* parent);
};
