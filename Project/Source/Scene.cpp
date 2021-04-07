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
	gameObject->RemoveAllComponents();
	gameObject->SetParent(nullptr);
	gameObjects.Release(gameObject);
}

GameObject* Scene::GetGameObject(UID id) const {
	if (gameObjectsIdMap.count(id) == 0) return nullptr;

	return gameObjectsIdMap.at(id);
}

Component* Scene::GetComponentByTypeAndId(ComponentType type, UID componentId) {
	switch (type) {
	case ComponentType::TRANSFORM:
		if (!transformComponents.Has(componentId)) return nullptr;
		return &transformComponents.Get(componentId);
	case ComponentType::MESH_RENDERER:
		if (!meshRendererComponents.Has(componentId)) return nullptr;
		return &meshRendererComponents.Get(componentId);
	case ComponentType::BOUNDING_BOX:
		if (!boundingBoxComponents.Has(componentId)) return nullptr;
		return &boundingBoxComponents.Get(componentId);
	case ComponentType::CAMERA:
		if (!cameraComponents.Has(componentId)) return nullptr;
		return &cameraComponents.Get(componentId);
	case ComponentType::LIGHT:
		if (!lightComponents.Has(componentId)) return nullptr;
		return &lightComponents.Get(componentId);
	case ComponentType::CANVAS:
		if (!canvasComponents.Has(componentId)) return nullptr;
		return &canvasComponents.Get(componentId);
	case ComponentType::CANVASRENDERER:
		if (!canvasRendererComponents.Has(componentId)) return nullptr;
		return &canvasRendererComponents.Get(componentId);
	case ComponentType::IMAGE:
		if (!imageComponents.Has(componentId)) return nullptr;
		return &imageComponents.Get(componentId);
	case ComponentType::TRANSFORM2D:
		if (!transform2DComponents.Has(componentId)) return nullptr;
		return &transform2DComponents.Get(componentId);
	case ComponentType::BUTTON:
		if (!buttonComponents.Has(componentId)) return nullptr;
		return &buttonComponents.Get(componentId);
	case ComponentType::EVENT_SYSTEM:
		if (!eventSystemComponents.Has(componentId)) return nullptr;
		return &eventSystemComponents.Get(componentId);
	case ComponentType::BOUNDING_BOX_2D:
		if (!boundingBox2DComponents.Has(componentId)) return nullptr;
		return &boundingBox2DComponents.Get(componentId);
	case ComponentType::TOGGLE:
		if (!toggleComponents.Has(componentId)) return nullptr;
		return &toggleComponents.Get(componentId);
	case ComponentType::TEXT:
		if (!textComponents.Has(componentId)) return nullptr;
		return &textComponents.Get(componentId);
	case ComponentType::SELECTABLE:
		if (!selectableComponents.Has(componentId)) return nullptr;
		return &selectableComponents.Get(componentId);
	case ComponentType::SKYBOX:
		if (!skyboxComponents.Has(componentId)) return nullptr;
		return &skyboxComponents.Get(componentId);
	case ComponentType::ANIMATION:
		if (!animationComponents.Has(componentId)) return nullptr;
		return &animationComponents.Get(componentId);
	default:
		LOG("Component of type %i hasn't been registered in GaneObject::GetComponentByTypeAndId.", (unsigned) type);
		assert(false);
		return nullptr;
	}
}

Component* Scene::CreateComponentByTypeAndId(GameObject* owner, ComponentType type, UID componentId) {
	switch (type) {
	case ComponentType::TRANSFORM:
		return &transformComponents.Put(componentId, ComponentTransform(owner, componentId, owner->IsActive()));
	case ComponentType::MESH_RENDERER:
		return &meshRendererComponents.Put(componentId, ComponentMeshRenderer(owner, componentId, owner->IsActive()));
	case ComponentType::BOUNDING_BOX:
		return &boundingBoxComponents.Put(componentId, ComponentBoundingBox(owner, componentId, owner->IsActive()));
	case ComponentType::CAMERA:
		return &cameraComponents.Put(componentId, ComponentCamera(owner, componentId, owner->IsActive()));
	case ComponentType::LIGHT:
		return &lightComponents.Put(componentId, ComponentLight(owner, componentId, owner->IsActive()));
	case ComponentType::CANVAS:
		return &canvasComponents.Put(componentId, ComponentCanvas(owner, componentId, owner->IsActive()));
	case ComponentType::CANVASRENDERER:
		return &canvasRendererComponents.Put(componentId, ComponentCanvasRenderer(owner, componentId, owner->IsActive()));
	case ComponentType::IMAGE:
		return &imageComponents.Put(componentId, ComponentImage(owner, componentId, owner->IsActive()));
	case ComponentType::TRANSFORM2D:
		return &transform2DComponents.Put(componentId, ComponentTransform2D(owner, componentId, owner->IsActive()));
	case ComponentType::BUTTON:
		return &buttonComponents.Put(componentId, ComponentButton(owner, componentId, owner->IsActive()));
	case ComponentType::EVENT_SYSTEM:
		return &eventSystemComponents.Put(componentId, ComponentEventSystem(owner, componentId, owner->IsActive()));
	case ComponentType::BOUNDING_BOX_2D:
		return &boundingBox2DComponents.Put(componentId, ComponentBoundingBox2D(owner, componentId, owner->IsActive()));
	case ComponentType::TOGGLE:
		return &toggleComponents.Put(componentId, ComponentToggle(owner, componentId, owner->IsActive()));
	case ComponentType::TEXT:
		return &textComponents.Put(componentId, ComponentText(owner, componentId, owner->IsActive()));
	case ComponentType::SELECTABLE:
		return &selectableComponents.Put(componentId, ComponentSelectable(owner, componentId, owner->IsActive()));
	case ComponentType::SKYBOX:
		return &skyboxComponents.Put(componentId, ComponentSkyBox(owner, componentId, owner->IsActive()));
	case ComponentType::ANIMATION:
		return &animationComponents.Put(componentId, ComponentAnimation(owner, componentId, owner->IsActive()));
	default:
		LOG("Component of type %i hasn't been registered in GameObject::CreateComponentByTypeAndId.", (unsigned) type);
		assert(false);
		return nullptr;
	}
}

void Scene::RemoveComponentByTypeAndId(ComponentType type, UID componentId) {
	switch (type) {
	case ComponentType::TRANSFORM:
		if (!transformComponents.Has(componentId)) return;
		transformComponents.Remove(componentId);
		break;
	case ComponentType::MESH_RENDERER:
		if (!meshRendererComponents.Has(componentId)) return;
		meshRendererComponents.Remove(componentId);
		break;
	case ComponentType::BOUNDING_BOX:
		if (!boundingBoxComponents.Has(componentId)) return;
		boundingBoxComponents.Remove(componentId);
		break;
	case ComponentType::CAMERA:
		if (!cameraComponents.Has(componentId)) return;
		cameraComponents.Remove(componentId);
		break;
	case ComponentType::LIGHT:
		if (!lightComponents.Has(componentId)) return;
		lightComponents.Remove(componentId);
		break;
	case ComponentType::CANVAS:
		if (!canvasComponents.Has(componentId)) return;
		canvasComponents.Remove(componentId);
		break;
	case ComponentType::CANVASRENDERER:
		if (!canvasRendererComponents.Has(componentId)) return;
		canvasRendererComponents.Remove(componentId);
		break;
	case ComponentType::IMAGE:
		if (!imageComponents.Has(componentId)) return;
		imageComponents.Remove(componentId);
		break;
	case ComponentType::TRANSFORM2D:
		if (!transform2DComponents.Has(componentId)) return;
		transform2DComponents.Remove(componentId);
		break;
	case ComponentType::BUTTON:
		if (!buttonComponents.Has(componentId)) return;
		buttonComponents.Remove(componentId);
		break;
	case ComponentType::EVENT_SYSTEM:
		if (!eventSystemComponents.Has(componentId)) return;
		eventSystemComponents.Remove(componentId);
		break;
	case ComponentType::BOUNDING_BOX_2D:
		if (!boundingBox2DComponents.Has(componentId)) return;
		boundingBox2DComponents.Remove(componentId);
		break;
	case ComponentType::TOGGLE:
		if (!toggleComponents.Has(componentId)) return;
		toggleComponents.Remove(componentId);
		break;
	case ComponentType::TEXT:
		if (!textComponents.Has(componentId)) return;
		textComponents.Remove(componentId);
		break;
	case ComponentType::SELECTABLE:
		if (!selectableComponents.Has(componentId)) return;
		selectableComponents.Remove(componentId);
		break;
	case ComponentType::SKYBOX:
		if (!skyboxComponents.Has(componentId)) return;
		skyboxComponents.Remove(componentId);
		break;
	case ComponentType::ANIMATION:
		if (!animationComponents.Has(componentId)) return;
		animationComponents.Remove(componentId);
		break;
	default:
		LOG("Component of type %i hasn't been registered in GameObject::RemoveComponentByTypeAndId.", (unsigned) type);
		assert(false);
		break;
	}
}

