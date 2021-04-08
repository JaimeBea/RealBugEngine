#pragma once

#include "Utils/Pool.h"
#include "Utils/VectorMap.h"
#include "Utils/Quadtree.h"
#include "Utils/UID.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentSkybox.h"
#include "Components/ComponentAnimation.h"

#include <unordered_map>

class GameObject;

class Scene {
public:
	Scene(unsigned numGameObjects);

	void ClearScene();		// Removes and clears every GameObject from the scene.
	void RebuildQuadtree(); // Recalculates the Quadtree hierarchy with all the GameObjects in the scene.
	void ClearQuadtree();	// Resets the Quadrtee as empty, and removes all GameObjects from it.

	// --- GameObject Management --- //
	GameObject* CreateGameObject(GameObject* parent, UID id, const char* name);
	GameObject* DuplicateGameObject(GameObject* gameObject, GameObject* parent);
	void DestroyGameObject(GameObject* gameObject);
	GameObject* GetGameObject(UID id) const;

public:
	GameObject* root = nullptr;							   // GameObject Root. Parent of everything and god among gods (Game Object Deity) :D.
	Pool<GameObject> gameObjects;						   // Pool of GameObjects. Stores all the memory of all existing GameObject in a contiguous memory space.
	std::unordered_map<UID, GameObject*> gameObjectsIdMap; // Maps every UID with the corresponding GameObject pointer.

	// ---- Components ---- //
	VectorMap<UID, ComponentTransform> transformComponents;
	VectorMap<UID, ComponentMeshRenderer> meshRendererComponents;
	VectorMap<UID, ComponentBoundingBox> boundingBoxComponents;
	VectorMap<UID, ComponentCamera> cameraComponents;
	VectorMap<UID, ComponentLight> lightComponents;
	VectorMap<UID, ComponentSkyBox> skyboxComponents;
	VectorMap<UID, ComponentAnimation> animationComponents;

	// ---- Quadtree Parameters ---- //
	Quadtree<GameObject> quadtree;
	AABB2D quadtreeBounds = {{-1000, -1000}, {1000, 1000}};
	unsigned quadtreeMaxDepth = 4;
	unsigned quadtreeElementsPerNode = 200;

	// TODO: (Texture resource) Make skybox work
	/*
	// Skybox
	unsigned skyboxVao = 0;
	unsigned skyboxVbo = 0;
	CubeMap* skyboxCubeMap = 0;
	*/
};
