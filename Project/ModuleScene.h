#pragma once

#include "Module.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "UID.h"
#include "Pool.h"

#include "assimp/cimport.h"
#include <unordered_map>

struct aiNode;
typedef unsigned int Texture;

class ModuleScene : public Module
{
public:
	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	bool Load(const char* file_name);

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* DuplicateGameObject(GameObject* parent);
	void DestroyGameObject(GameObject* game_object);
	GameObject* GetGameObject(UID id) const;

public:
	GameObject* root = nullptr;

private:
	GameObject* LoadNode(const aiScene* scene, const std::vector<Texture*>& materials, const aiNode* node, GameObject* parent);

private:
	Pool<GameObject> game_objects = Pool<GameObject>();
	std::unordered_map<UID, GameObject*> game_objects_id_map = std::unordered_map<UID, GameObject*>();

	aiLogStream log_stream = {nullptr, nullptr};
};
