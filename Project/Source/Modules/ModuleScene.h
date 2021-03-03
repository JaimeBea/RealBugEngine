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
	// ------- Core Functions ------ //
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void CreateEmptyScene(); // Crates a new scene with a default game camera and directional light.
	void ClearScene();		 // Removes and clears every GameObject from the scene. Used in CreateEmptyScene().
	void RebuildQuadtree();	 // Recalculates the Quadtree hierarchy with all the GameObjects in the scene.
	void ClearQuadtree();	 // Resets the Quadrtee as empty, and removes all GameObjects from it.

	// --- GameObject Management --- //
	GameObject* CreateGameObject(GameObject* parent);
	GameObject* DuplicateGameObject(GameObject* gameObject, GameObject* parent);
	void DestroyGameObject(GameObject* gameObject);
	GameObject* GetGameObject(UID id) const;

public:
	std::string fileName = "";							   // REVIEW. This can be removed? Is it even used for anything?
	GameObject* root = nullptr;							   // GameObject Root. Parent of everything and god among gods (Game Object Deity) :D.
	Pool<GameObject> gameObjects;						   // Pool of GameObjects. Stores all the memory of all existing GameObject in a contiguous memory space.
	std::unordered_map<UID, GameObject*> gameObjectsIdMap; // Maps every UID with the corresponding GameObject pointer.

	// ---- Quadtree Parameters ---- //
	Quadtree<GameObject> quadtree;
	AABB2D quadtreeBounds = {{-1000, -1000}, {1000, 1000}};
	unsigned quadtreeMaxDepth = 4;
	unsigned quadtreeElementsPerNode = 200;

	// ---- SkyBox GL Pointers ----- //
	unsigned skyboxVao = 0;
	unsigned skyboxVbo = 0;
	CubeMap* skyboxCubeMap = 0;
};
