#pragma once

#include "Module.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Pool.h"

#include "assimp/cimport.h"
#include <vector>

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

public:
	GameObject* root = nullptr;

	Pool<GameObject> game_objects = Pool<GameObject>();

private:
	GameObject* LoadNode(const aiScene* scene, const std::vector<Texture*>& materials, const aiNode* node, GameObject* parent);

private:
	aiLogStream log_stream = { nullptr, nullptr };
};
