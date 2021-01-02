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

	Pool<GameObject> game_objects = Pool<GameObject>();

private:
	GameObject* LoadNode(const aiScene* scene, const std::vector<Texture*>& materials, const aiNode* node, GameObject* parent);
	void LoadSkyBox();

public:
	void DrawSkyBox();

private:
	Pool<GameObject> game_objects = Pool<GameObject>();
	std::unordered_map<UID, GameObject*> game_objects_id_map = std::unordered_map<UID, GameObject*>();

	// Skybox
	unsigned skybox_vao = 0;
	unsigned skybox_vbo = 0;
	Texture* skybox_texture = 0;
};
