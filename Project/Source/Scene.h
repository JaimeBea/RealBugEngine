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
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentCanvasRenderer.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentToggle.h"
#include "Components/UI/ComponentText.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Components/ComponentSkybox.h"
#include "Components/ComponentScript.h"
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

	// --- Component Management --- //
	TESSERACT_ENGINE_API Component* GetComponentByTypeAndId(ComponentType type, UID componentId);
	Component* CreateComponentByTypeAndId(GameObject* owner, ComponentType type, UID componentId);
	void AddComponent(const Component* component);
	void RemoveComponentByTypeAndId(ComponentType type, UID componentId);

	template<class T> T* GetComponent(UID id);

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
	VectorMap<UID, ComponentCanvas> canvasComponents;
	VectorMap<UID, ComponentCanvasRenderer> canvasRendererComponents;
	VectorMap<UID, ComponentImage> imageComponents;
	VectorMap<UID, ComponentTransform2D> transform2DComponents;
	VectorMap<UID, ComponentBoundingBox2D> boundingBox2DComponents;
	VectorMap<UID, ComponentEventSystem> eventSystemComponents;
	VectorMap<UID, ComponentToggle> toggleComponents;
	VectorMap<UID, ComponentText> textComponents;
	VectorMap<UID, ComponentButton> buttonComponents;
	VectorMap<UID, ComponentSelectable> selectableComponents;
	VectorMap<UID, ComponentSkyBox> skyboxComponents;
	VectorMap<UID, ComponentScript> scriptComponents;
	VectorMap<UID, ComponentAnimation> animationComponents;

	// ---- Quadtree Parameters ---- //
	Quadtree<GameObject> quadtree;
	AABB2D quadtreeBounds = {{-1000, -1000}, {1000, 1000}};
	unsigned quadtreeMaxDepth = 4;
	unsigned quadtreeElementsPerNode = 200;
};

template<class T>
inline T* Scene::GetComponent(UID id) {
	return (T*) GetComponentByTypeAndId(T::staticType, id);
}