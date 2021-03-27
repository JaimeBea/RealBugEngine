#include "GameObject.h"

#include "Globals.h"
#include "Components/ComponentType.h"

#include "Math/myassert.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

#define JSON_TAG_ID "Id"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_ACTIVE "Active"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_COMPONENTS "Components"
#define JSON_TAG_CHILDREN "Children"

void GameObject::InitComponents() {
	for (const std::pair<ComponentType, UID>& pair : components) {
		Component* component = GetComponentByTypeAndId(pair.first, pair.second);
		component->Init();
	}
}

void GameObject::Update() {
	if (IsActiveInHierarchy()) {
		for (const std::pair<ComponentType, UID>& pair : components) {
			Component* component = GetComponentByTypeAndId(pair.first, pair.second);
			component->Update();
		}

		for (GameObject* child : children) {
			child->Update();
		}
	}
}

void GameObject::DrawGizmos() {
	for (const std::pair<ComponentType, UID>& pair : components) {
		Component* component = GetComponentByTypeAndId(pair.first, pair.second);
		component->DrawGizmos();
	}

	for (GameObject* child : children) {
		child->DrawGizmos();
	}
}

void GameObject::Enable() {
	active = true;
}

void GameObject::Disable() {
	active = false;
}

bool GameObject::IsActive() const {
	return active;
}

bool GameObject::IsActiveInHierarchy() const {
	if (parent) return parent->IsActiveInHierarchy() && active;

	return active;
}

UID GameObject::GetID() const {
	return id;
}

std::vector<Component*> GameObject::GetComponents() const {
	std::vector<Component*> auxComponents;

	for (const std::pair<ComponentType, UID>& pair : components) {
		auxComponents.push_back(GetComponentByTypeAndId(pair.first, pair.second));
	}

	return auxComponents;
}

void GameObject::RemoveComponent(Component* component) {
	for (auto it = components.begin(); it != components.end(); ++it) {
		if (it->second == component->GetID()) {
			RemoveComponentByTypeAndId(it->first, it->second);
			components.erase(it);
			break;
		}
	}
}

void GameObject::RemoveAllComponents() {
	while (!components.empty()) {
		std::pair<ComponentType, UID> pair = components.back();
		RemoveComponentByTypeAndId(pair.first, pair.second);
		components.pop_back();
	}
}

void GameObject::SetParent(GameObject* gameObject) {
	if (parent != nullptr) {
		bool found = false;
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it) {
			if (*it == this) {
				found = true;
				parent->children.erase(it);
				break;
			}
		}
		assert(found);
	}
	parent = gameObject;
	if (gameObject != nullptr) {
		gameObject->children.push_back(this);
	}
}

GameObject* GameObject::GetParent() const {
	return parent;
}

void GameObject::AddChild(GameObject* gameObject) {
	gameObject->SetParent(this);
}

void GameObject::RemoveChild(GameObject* gameObject) {
	gameObject->SetParent(nullptr);
}

const std::vector<GameObject*>& GameObject::GetChildren() const {
	return children;
}

bool GameObject::IsDescendantOf(GameObject* gameObject) {
	if (GetParent() == nullptr) return false;
	if (GetParent() == gameObject) return true;
	return GetParent()->IsDescendantOf(gameObject);
}

bool GameObject::HasChildren() const {
	return !children.empty();
}

void GameObject::Save(JsonValue jGameObject) const {
	jGameObject[JSON_TAG_ID] = id;
	jGameObject[JSON_TAG_NAME] = name.c_str();
	jGameObject[JSON_TAG_ACTIVE] = active;

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < components.size(); ++i) {
		JsonValue jComponent = jComponents[i];
		std::pair<ComponentType, UID> pair = components[i];
		Component* component = GetComponentByTypeAndId(pair.first, pair.second);

		jComponent[JSON_TAG_TYPE] = (unsigned) component->GetType();
		jComponent[JSON_TAG_ID] = component->GetID();
		jComponent[JSON_TAG_ACTIVE] = component->IsActive();
		component->Save(jComponent);
	}

	JsonValue jChildren = jGameObject[JSON_TAG_CHILDREN];
	for (unsigned i = 0; i < children.size(); ++i) {
		JsonValue jChild = jChildren[i];
		GameObject* child = children[i];
		child->Save(jChild);
	}
}

void GameObject::Load(JsonValue jGameObject) {
	id = jGameObject[JSON_TAG_ID];
	name = jGameObject[JSON_TAG_NAME];
	active = jGameObject[JSON_TAG_ACTIVE];

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < jComponents.Size(); ++i) {
		JsonValue jComponent = jComponents[i];

		ComponentType type = (ComponentType)(unsigned) jComponent[JSON_TAG_TYPE];
		UID componentId = jComponent[JSON_TAG_ID];
		bool active = jComponent[JSON_TAG_ACTIVE];

		components.push_back(std::pair<ComponentType, UID>(type, componentId));
		Component* component = CreateComponentByTypeAndId(type, componentId);
		component->Load(jComponent);
	}

	JsonValue jChildren = jGameObject[JSON_TAG_CHILDREN];
	for (unsigned i = 0; i < jChildren.Size(); ++i) {
		JsonValue jChild = jChildren[i];
		std::string childName = jChild[JSON_TAG_NAME];

		GameObject* child = scene->gameObjects.Obtain();
		child->scene = scene;
		child->Load(jChild);
		scene->gameObjectsIdMap[child->id] = child;
		child->SetParent(this);
		child->InitComponents();
	}
}

void GameObject::SavePrototype(JsonValue jGameObject) const {
	jGameObject[JSON_TAG_NAME] = name.c_str();
	jGameObject[JSON_TAG_ACTIVE] = active;

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < components.size(); ++i) {
		JsonValue jComponent = jComponents[i];
		std::pair<ComponentType, UID> pair = components[i];
		Component* component = GetComponentByTypeAndId(pair.first, pair.second);

		jComponent[JSON_TAG_TYPE] = (unsigned) component->GetType();
		jComponent[JSON_TAG_ACTIVE] = component->IsActive();
		component->Save(jComponent);
	}

	JsonValue jChildren = jGameObject[JSON_TAG_CHILDREN];
	for (unsigned i = 0; i < children.size(); ++i) {
		JsonValue jChild = jChildren[i];
		GameObject* child = children[i];

		child->SavePrototype(jChild);
	}
}

void GameObject::LoadPrototype(JsonValue jGameObject) {
	name = jGameObject[JSON_TAG_NAME];
	active = jGameObject[JSON_TAG_ACTIVE];

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < jComponents.Size(); ++i) {
		JsonValue jComponent = jComponents[i];

		ComponentType type = (ComponentType)(unsigned) jComponent[JSON_TAG_TYPE];
		UID componentId = GenerateUID();
		bool active = jComponent[JSON_TAG_ACTIVE];

		components.push_back(std::pair<ComponentType, UID>(type, componentId));
		Component* component = CreateComponentByTypeAndId(type, componentId);
		component->Load(jComponent);
	}

	JsonValue jChildren = jGameObject[JSON_TAG_CHILDREN];
	for (unsigned i = 0; i < jChildren.Size(); ++i) {
		JsonValue jChild = jChildren[i];
		std::string childName = jChild[JSON_TAG_NAME];

		GameObject* child = scene->gameObjects.Obtain();
		child->scene = scene;
		child->LoadPrototype(jChild);
		child->id = GenerateUID();
		scene->gameObjectsIdMap[child->id] = child;
		child->SetParent(this);
		child->InitComponents();
	}
}

Component* GameObject::GetComponentByTypeAndId(ComponentType type, UID componentId) const {
	switch (type) {
	case ComponentType::TRANSFORM:
		if (!scene->transformComponents.Has(componentId)) return nullptr;
		return &scene->transformComponents.Get(componentId);
	case ComponentType::MESH_RENDERER:
		if (!scene->meshRendererComponents.Has(componentId)) return nullptr;
		return &scene->meshRendererComponents.Get(componentId);
	case ComponentType::BOUNDING_BOX:
		if (!scene->boundingBoxComponents.Has(componentId)) return nullptr;
		return &scene->boundingBoxComponents.Get(componentId);
	case ComponentType::CAMERA:
		if (!scene->cameraComponents.Has(componentId)) return nullptr;
		return &scene->cameraComponents.Get(componentId);
	case ComponentType::LIGHT:
		if (!scene->lightComponents.Has(componentId)) return nullptr;
		return &scene->lightComponents.Get(componentId);
	case ComponentType::SKYBOX:
		if (!scene->skyboxComponents.Has(componentId)) return nullptr;
		return &scene->skyboxComponents.Get(componentId);
	default:
		LOG("Component of type %i hasn't been registered in GaneObject::GetComponentByTypeAndId.", (unsigned) type);
		assert(false);
		return nullptr;
	}
}

Component* GameObject::CreateComponentByTypeAndId(ComponentType type, UID componentId) {
	switch (type) {
	case ComponentType::TRANSFORM:
		return &scene->transformComponents.Put(componentId, ComponentTransform(this, componentId, active));
	case ComponentType::MESH_RENDERER:
		return &scene->meshRendererComponents.Put(componentId, ComponentMeshRenderer(this, componentId, active));
	case ComponentType::BOUNDING_BOX:
		return &scene->boundingBoxComponents.Put(componentId, ComponentBoundingBox(this, componentId, active));
	case ComponentType::CAMERA:
		return &scene->cameraComponents.Put(componentId, ComponentCamera(this, componentId, active));
	case ComponentType::LIGHT:
		return &scene->lightComponents.Put(componentId, ComponentLight(this, componentId, active));
	case ComponentType::SKYBOX:
		return &scene->skyboxComponents.Put(componentId, ComponentSkyBox(this, componentId, active));
	default:
		LOG("Component of type %i hasn't been registered in GameObject::CreateComponentByTypeAndId.", (unsigned) type);
		assert(false);
		return nullptr;
	}
}

void GameObject::RemoveComponentByTypeAndId(ComponentType type, UID componentId) {
	switch (type) {
	case ComponentType::TRANSFORM:
		if (!scene->transformComponents.Has(componentId)) return;
		scene->transformComponents.Remove(componentId);
		break;
	case ComponentType::MESH_RENDERER:
		if (!scene->meshRendererComponents.Has(componentId)) return;
		scene->meshRendererComponents.Remove(componentId);
		break;
	case ComponentType::BOUNDING_BOX:
		if (!scene->boundingBoxComponents.Has(componentId)) return;
		scene->boundingBoxComponents.Remove(componentId);
		break;
	case ComponentType::CAMERA:
		if (!scene->cameraComponents.Has(componentId)) return;
		scene->cameraComponents.Remove(componentId);
		break;
	case ComponentType::LIGHT:
		if (!scene->lightComponents.Has(componentId)) return;
		scene->lightComponents.Remove(componentId);
		break;
	case ComponentType::SKYBOX:
		if (!scene->skyboxComponents.Has(componentId)) return;
		scene->skyboxComponents.Remove(componentId);
		break;
	default:
		LOG("Component of type %i hasn't been registered in GameObject::RemoveComponentByTypeAndId.", (unsigned) type);
		assert(false);
		break;
	}
}