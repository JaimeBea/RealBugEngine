#pragma once

#include "Modules/Module.h"
#include "Resources/GameObject.h"
#include "Utils/UID.h"
#include "Utils/Pool.h"
#include "Utils/Quadtree.h"

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

	void CreateEmptyScene();
	void ClearScene();
	void RebuildQuadtree();
	void ClearQuadtree();

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* DuplicateGameObject(GameObject* parent);
	void DestroyGameObject(GameObject* game_object);
	GameObject* GetGameObject(UID id) const;

public:
	std::string file_name = "";
	GameObject* root = nullptr;

	Pool<GameObject> game_objects;
	std::unordered_map<UID, GameObject*> game_objects_id_map;

	// Quadtree
	Quadtree<GameObject> quadtree;
	AABB2D quadtree_bounds = {{-1000, -1000}, {1000, 1000}};
	unsigned quadtree_max_depth = 4;
	unsigned quadtree_elements_per_node = 200;

	// Skybox
	unsigned skybox_vao = 0;
	unsigned skybox_vbo = 0;
	CubeMap* skybox_cube_map = 0;
};
