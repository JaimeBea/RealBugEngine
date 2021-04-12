#include "GameObject.h"

#include "Globals.h"
#include "Components/ComponentType.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentCanvasRenderer.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Components/UI/ComponentSelectable.h"
#include "FileSystem/ModelImporter.h"

#include "Math/myassert.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

#define JSON_TAG_ID "Id"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_ACTIVE "Active"
#define JSON_TAG_ROOT_BONE_ID "RootBoneId"
#define JSON_TAG_ROOT_BONE_NAME "RootBoneName"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_COMPONENTS "Components"
#define JSON_TAG_CHILDREN "Children"

void GameObject::InitComponents() {
	for (const std::pair<ComponentType, UID>& pair : components) {
		Component* component = scene->GetComponentByTypeAndId(pair.first, pair.second);
		component->Init();
	}
}

void GameObject::Update() {
	if (IsActiveInHierarchy()) {
		for (const std::pair<ComponentType, UID>& pair : components) {
			Component* component = scene->GetComponentByTypeAndId(pair.first, pair.second);
			component->Update();
		}

		for (GameObject* child : children) {
			child->Update();
		}
	}
}

void GameObject::DrawGizmos() {
	for (const std::pair<ComponentType, UID>& pair : components) {
		Component* component = scene->GetComponentByTypeAndId(pair.first, pair.second);
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
		auxComponents.push_back(scene->GetComponentByTypeAndId(pair.first, pair.second));
	}

	return auxComponents;
}

void GameObject::RemoveComponent(Component* component) {
	for (auto it = components.begin(); it != components.end(); ++it) {
		if (it->second == component->GetID()) {
			scene->RemoveComponentByTypeAndId(it->first, it->second);
			components.erase(it);
			break;
		}
	}
}

void GameObject::RemoveAllComponents() {
	while (!components.empty()) {
		std::pair<ComponentType, UID> pair = components.back();
		scene->RemoveComponentByTypeAndId(pair.first, pair.second);
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

void GameObject::SetRootBone(GameObject* gameObject) {
	rootBoneHierarchy = gameObject;
}

GameObject* GameObject::GetRootBone() const {
	return rootBoneHierarchy;
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

GameObject* GameObject::FindDescendant(const std::string& name) const {
	for (GameObject* child : children) {
		if (child->name == name) {
			return child;
		} else {
			GameObject* gameObject = child->FindDescendant(name);
			if (gameObject != nullptr) return gameObject;
		}
	}

	return nullptr;
}

bool GameObject::HasChildren() const {
	return !children.empty();
}

void GameObject::Save(JsonValue jGameObject) const {
	jGameObject[JSON_TAG_ID] = id;
	jGameObject[JSON_TAG_NAME] = name.c_str();
	jGameObject[JSON_TAG_ACTIVE] = active;
	jGameObject[JSON_TAG_ROOT_BONE_ID] = rootBoneHierarchy != nullptr ? rootBoneHierarchy->id : 0;

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < components.size(); ++i) {
		JsonValue jComponent = jComponents[i];
		std::pair<ComponentType, UID> pair = components[i];
		Component* component = scene->GetComponentByTypeAndId(pair.first, pair.second);

		jComponent[JSON_TAG_TYPE] = GetComponentTypeName(component->GetType());
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

		std::string typeName = jComponent[JSON_TAG_TYPE];
		UID componentId = jComponent[JSON_TAG_ID];
		bool active = jComponent[JSON_TAG_ACTIVE];

		ComponentType type = GetComponentTypeFromName(typeName.c_str());
		components.push_back(std::pair<ComponentType, UID>(type, componentId));
		Component* component = scene->CreateComponentByTypeAndId(this, type, componentId);
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

	UID rootBoneId = jGameObject[JSON_TAG_ROOT_BONE_ID];
	rootBoneHierarchy = scene->GetGameObject(rootBoneId);
	// Recache bones to unordered map
	if (rootBoneHierarchy) {
		std::unordered_map<std::string, GameObject*> temporalBonesMap;
		ModelImporter::CacheBones(rootBoneHierarchy, temporalBonesMap);
		ModelImporter::SaveBones(this, temporalBonesMap);
	}
}

void GameObject::SavePrototype(JsonValue jGameObject) const {
	jGameObject[JSON_TAG_NAME] = name.c_str();
	jGameObject[JSON_TAG_ACTIVE] = active;
	jGameObject[JSON_TAG_ROOT_BONE_NAME] = rootBoneHierarchy ? rootBoneHierarchy->name.c_str() : "";

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < components.size(); ++i) {
		JsonValue jComponent = jComponents[i];
		std::pair<ComponentType, UID> pair = components[i];
		Component* component = scene->GetComponentByTypeAndId(pair.first, pair.second);

		jComponent[JSON_TAG_TYPE] = GetComponentTypeName(component->GetType());
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

		std::string typeName = jComponent[JSON_TAG_TYPE];
		UID componentId = GenerateUID();
		bool active = jComponent[JSON_TAG_ACTIVE];

		ComponentType type = GetComponentTypeFromName(typeName.c_str());
		components.push_back(std::pair<ComponentType, UID>(type, componentId));
		Component* component = scene->CreateComponentByTypeAndId(this, type, componentId);
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

	std::string rootBoneName = jGameObject[JSON_TAG_ROOT_BONE_NAME];
	if (rootBoneName != "") {
		rootBoneHierarchy = (rootBoneName == this->name) ? this : FindDescendant(rootBoneName);
		// Recache bones to unordered map
		std::unordered_map<std::string, GameObject*> temporalBonesMap;
		ModelImporter::CacheBones(rootBoneHierarchy, temporalBonesMap);
		ModelImporter::SaveBones(this, temporalBonesMap);
	}
}
