#pragma once

#include "Module.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "UID.h"
#include "Pool.h"

#include <unordered_map>
#include <vector>

class CubeMap;
struct aiScene;
struct aiNode;

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

	// Skybox
	unsigned skybox_vao = 0;
	unsigned skybox_vbo = 0;
	CubeMap* skybox_cube_map = 0;

private:
	Pool<GameObject> game_objects = Pool<GameObject>();
	std::unordered_map<UID, GameObject*> game_objects_id_map = std::unordered_map<UID, GameObject*>();
};
