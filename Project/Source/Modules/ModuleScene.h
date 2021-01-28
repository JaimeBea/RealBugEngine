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

class ModuleScene : public Module {
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
	std::string fileName = "";
	GameObject* root = nullptr;

	Pool<GameObject> gameObjects;
	std::unordered_map<UID, GameObject*> gameObjectsIdMap;

	// Quadtree
	Quadtree<GameObject> quadtree;
	AABB2D quadtreeBounds = {{-1000, -1000}, {1000, 1000}};
	unsigned quadtreeMaxDepth = 4;
	unsigned quadtreeElementsPerNode = 200;

	// Skybox
	unsigned skyboxVao = 0;
	unsigned skyboxVbo = 0;
	CubeMap* skyboxCubeMap = 0;
};
