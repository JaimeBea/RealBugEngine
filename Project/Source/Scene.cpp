#include "Scene.h"

#include "GameObject.h"

Scene::Scene(unsigned numGameObjects) {
	gameObjects.Allocate(numGameObjects);
}

void Scene::ClearScene() {
	DestroyGameObject(root);
	root = nullptr;
	quadtree.Clear();

	assert(gameObjects.Count() == 0); // There should be no GameObjects outside the scene hierarchy
	gameObjects.ReleaseAll();		  // This looks redundant, but it resets the free list so that GameObject order is mantained when saving/loading
}

void Scene::RebuildQuadtree() {
	quadtree.Initialize(quadtreeBounds, quadtreeMaxDepth, quadtreeElementsPerNode);
	for (ComponentBoundingBox& boundingBox : boundingBoxComponents) {
		GameObject& gameObject = boundingBox.GetOwner();
		boundingBox.CalculateWorldBoundingBox();
		const AABB& worldAABB = boundingBox.GetWorldAABB();
		quadtree.Add(&gameObject, AABB2D(worldAABB.minPoint.xz(), worldAABB.maxPoint.xz()));
		gameObject.isInQuadtree = true;
	}
	quadtree.Optimize();
}

void Scene::ClearQuadtree() {
	quadtree.Clear();
	for (GameObject& gameObject : gameObjects) {
		gameObject.isInQuadtree = false;
	}
}

GameObject* Scene::CreateGameObject(GameObject* parent, UID id, const char* name) {
	GameObject* gameObject = gameObjects.Obtain();
	gameObject->scene = this;
	gameObject->id = id;
	gameObject->name = name;
	gameObjectsIdMap[id] = gameObject;
	gameObject->SetParent(parent);

	return gameObject;
}

GameObject* Scene::DuplicateGameObject(GameObject* gameObject, GameObject* parent) {
	GameObject* newGO = CreateGameObject(parent, GenerateUID(), (gameObject->name + " (copy)").c_str());

	// Copy the components
	for (Component* component : gameObject->GetComponents()) {
		component->DuplicateComponent(*newGO);
	}
	newGO->InitComponents();
	// Duplicate recursively its children
	for (GameObject* child : gameObject->GetChildren()) {
		DuplicateGameObject(child, newGO);
	}
	return newGO;
}

void Scene::DestroyGameObject(GameObject* gameObject) {
	if (gameObject == nullptr) return;

	// We need a copy because we are invalidating the iterator by removing GameObjects
	std::vector<GameObject*> children = gameObject->GetChildren();
	for (GameObject* child : children) {
		DestroyGameObject(child);
	}

	if (gameObject->isInQuadtree) {
		quadtree.Remove(gameObject);
	}

	gameObjectsIdMap.erase(gameObject->GetID());
	gameObject->RemoveComponents();
	gameObject->SetParent(nullptr);
	gameObjects.Release(gameObject);
}

GameObject* Scene::GetGameObject(UID id) const {
	if (gameObjectsIdMap.count(id) == 0) return nullptr;

	return gameObjectsIdMap.at(id);
}
