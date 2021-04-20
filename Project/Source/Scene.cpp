#include "Scene.h"

#include "GameObject.h"
#include "Resources/ResourceMesh.h"

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
	case ComponentType::SCRIPT:
		if (!scriptComponents.Has(componentId)) return nullptr;
		return &scriptComponents.Get(componentId);
	default:
		LOG("Component of type %i hasn't been registered in Scene::GetComponentByTypeAndId.", (unsigned) type);
		assert(false);
		return nullptr;
	}
}

Component* Scene::CreateComponentByTypeAndId(GameObject* owner, ComponentType type, UID componentId) {
	switch (type) {
	case ComponentType::TRANSFORM:
		return &transformComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::MESH_RENDERER:
		return &meshRendererComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::BOUNDING_BOX:
		return &boundingBoxComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::CAMERA:
		return &cameraComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::LIGHT:
		return &lightComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::CANVAS:
		return &canvasComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::CANVASRENDERER:
		return &canvasRendererComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::IMAGE:
		return &imageComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::TRANSFORM2D:
		return &transform2DComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::BUTTON:
		return &buttonComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::EVENT_SYSTEM:
		return &eventSystemComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::BOUNDING_BOX_2D:
		return &boundingBox2DComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::TOGGLE:
		return &toggleComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::TEXT:
		return &textComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::SELECTABLE:
		return &selectableComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::SKYBOX:
		return &skyboxComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::ANIMATION:
		return &animationComponents.Put(componentId, owner, componentId, owner->IsActive());
	case ComponentType::SCRIPT:
		return &scriptComponents.Put(componentId, owner, componentId, owner->IsActive());
	default:
		LOG("Component of type %i hasn't been registered in Scene::CreateComponentByTypeAndId.", (unsigned) type);
		assert(false);
		return nullptr;
	}
}

void Scene::AddComponent(const Component* component) {
	component->GetOwner().components.push_back(std::pair<ComponentType, UID>(component->GetType(), component->GetID()));

	switch (component->GetType()) {
	case ComponentType::TRANSFORM:
		transformComponents.Put(component->GetID(), (const ComponentTransform&) *component).Init();
		break;
	case ComponentType::MESH_RENDERER:
		meshRendererComponents.Put(component->GetID(), (const ComponentMeshRenderer&) *component).Init();
		break;
	case ComponentType::BOUNDING_BOX:
		boundingBoxComponents.Put(component->GetID(), (const ComponentBoundingBox&) *component).Init();
		break;
	case ComponentType::CAMERA:
		cameraComponents.Put(component->GetID(), (const ComponentCamera&) *component).Init();
		break;
	case ComponentType::LIGHT:
		lightComponents.Put(component->GetID(), (const ComponentLight&) *component).Init();
		break;
	case ComponentType::CANVAS:
		canvasComponents.Put(component->GetID(), (const ComponentCanvas&) *component).Init();
		break;
	case ComponentType::CANVASRENDERER:
		canvasRendererComponents.Put(component->GetID(), (const ComponentCanvasRenderer&) *component).Init();
		break;
	case ComponentType::IMAGE:
		imageComponents.Put(component->GetID(), (const ComponentImage&) *component).Init();
		break;
	case ComponentType::TRANSFORM2D:
		transform2DComponents.Put(component->GetID(), (const ComponentTransform2D&) *component).Init();
		break;
	case ComponentType::BUTTON:
		buttonComponents.Put(component->GetID(), (const ComponentButton&) *component).Init();
		break;
	case ComponentType::EVENT_SYSTEM:
		eventSystemComponents.Put(component->GetID(), (const ComponentEventSystem&) *component).Init();
		break;
	case ComponentType::BOUNDING_BOX_2D:
		boundingBox2DComponents.Put(component->GetID(), (const ComponentBoundingBox2D&) *component).Init();
		break;
	case ComponentType::TOGGLE:
		toggleComponents.Put(component->GetID(), (const ComponentToggle&) *component).Init();
		break;
	case ComponentType::TEXT:
		textComponents.Put(component->GetID(), (const ComponentText&) *component).Init();
		break;
	case ComponentType::SELECTABLE:
		selectableComponents.Put(component->GetID(), (const ComponentSelectable&) *component).Init();
		break;
	case ComponentType::SKYBOX:
		skyboxComponents.Put(component->GetID(), (const ComponentSkyBox&) *component).Init();
		break;
	case ComponentType::ANIMATION:
		animationComponents.Put(component->GetID(), (const ComponentAnimation&) *component).Init();
		break;
	case ComponentType::SCRIPT:
		scriptComponents.Put(component->GetID(), (const ComponentScript&) *component).Init();
		break;
	default:
		LOG("Component of type %i hasn't been registered in Scene::AddComponent.", (unsigned) component->GetType());
		assert(false);
		break;
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
	case ComponentType::SCRIPT:
		if (!scriptComponents.Has(componentId)) return;
		scriptComponents.Remove(componentId);
		break;
	default:
		LOG("Component of type %i hasn't been registered in Scene::RemoveComponentByTypeAndId.", (unsigned) type);
		assert(false);
		break;
	}
}

int Scene::GetTotalTriangles() const {
	int triangles = 0;
	for (const ComponentMeshRenderer& meshComponent : meshRendererComponents) {
		ResourceMesh* mesh = (ResourceMesh*) App->resources->GetResource(meshComponent.meshId);
		triangles += mesh->numIndices / 3;
	}
	return triangles;
}
