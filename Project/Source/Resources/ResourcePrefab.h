#pragma once

#include "Resources/Resource.h"

class GameObject;

class ResourcePrefab : public Resource {
public:
	REGISTER_RESOURCE(ResourcePrefab, ResourceType::PREFAB);

	void BuildPrefab(GameObject* parent);
	void ReloadPrefab(GameObject* gameObject);
	void SavePrefab(GameObject* gameObject);
};
