#pragma once

#include "Module.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "UID.h"
#include "Pool.h"

#include <unordered_map>
#include <string>

class CubeMap;
struct aiScene;
struct aiNode;

class ModuleScene : public Module
{
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void ClearScene();

	void DrawSkyBox();

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* DuplicateGameObject(GameObject* parent);
	void DestroyGameObject(GameObject* game_object);
	GameObject* GetGameObject(UID id) const;

public:
	std::string file_name = "";
	GameObject* root = nullptr;

	Pool<GameObject> game_objects = Pool<GameObject>();
	std::list<GameObject*> scene_cameras;

private:
	std::unordered_map<UID, GameObject*> game_objects_id_map = std::unordered_map<UID, GameObject*>();

	// Skybox
	unsigned skybox_vao = 0;
	unsigned skybox_vbo = 0;
	CubeMap* skybox_cube_map = 0;
};
