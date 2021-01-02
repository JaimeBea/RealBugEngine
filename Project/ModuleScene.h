#pragma once

#include "Module.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "UID.h"
#include "Pool.h"

#include "assimp/cimport.h"
#include <unordered_map>

class ModuleScene : public Module
{
public:
	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	bool Import(const char* file_name);
	bool Load(const char* file_name);
	bool Save(const char* file_name) const;

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* DuplicateGameObject(GameObject* parent);
	void DestroyGameObject(GameObject* game_object);
	GameObject* GetGameObject(UID id) const;

public:
	GameObject* root = nullptr;

private:
	Pool<GameObject> game_objects = Pool<GameObject>();
	std::unordered_map<UID, GameObject*> game_objects_id_map = std::unordered_map<UID, GameObject*>();
};
