#pragma once

#pragma warning(disable : 4251)

#include "Components/Component.h"
#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEvents.h"
#include "Utils/Logging.h"
#include "Utils/UID.h"
#include "FileSystem/JsonValue.h"
#include "TesseractEvent.h"

#include "Math/myassert.h"
#include <vector>
#include <string>

class TESSERACT_ENGINE_API GameObject {
public:
	void InitComponents();
	void Update();
	void DrawGizmos();

	void Enable();
	void Disable();
	bool IsActive() const;
	bool IsActiveInHierarchy() const;

	UID GetID() const;

	template<class T> T* CreateComponent();
	template<class T> T* CreateComponentDeferred();
	template<class T> bool HasComponent() const;
	template<class T> T* GetComponent() const;
	template<class T> std::vector<T*> GetComponents() const;
	template<class T> GameObject* HasComponentInAnyParent(GameObject* current) const; // Finds in the current object or in any parent of this Object the T Component. Returns the GameObject if found, else nullptr
	std::vector<Component*> GetComponents() const;
	void RemoveComponent(Component* component);
	void RemoveAllComponents();

	void SetParent(GameObject* gameObject);
	GameObject* GetParent() const;

	void SetRootBone(GameObject* gameObject);
	GameObject* GetRootBone() const;

	void AddChild(GameObject* gameObject);
	void RemoveChild(GameObject* gameObject);
	const std::vector<GameObject*>& GetChildren() const;
	bool IsDescendantOf(GameObject* gameObject);
	GameObject* FindDescendant(const std::string& name) const;
	bool HasChildren() const;

	void Save(JsonValue jGameObject) const;
	void Load(JsonValue jGameObject);

	void SavePrototype(JsonValue jGameObject) const;
	void LoadPrototype(JsonValue jGameObject);

public:
	UID id = 0;
	std::string name = "";

	Scene* scene = nullptr;
	bool isInQuadtree = false;

	bool flag = false; // Auxiliary variable to help with iterating on the Quadtree

	std::vector<std::pair<ComponentType, UID>> components;

private:
	bool active = true;
	GameObject* parent = nullptr;
	GameObject* rootBoneHierarchy = nullptr;
	std::vector<GameObject*> children;
};

template<class T>
inline T* GameObject::CreateComponent() {
	if (!T::allowMultipleComponents && HasComponent<T>()) return nullptr;
	UID componentId = GenerateUID();
	components.push_back(std::pair<ComponentType, UID>(T::staticType, componentId));
	return (T*) scene->CreateComponentByTypeAndId(this, T::staticType, componentId);
}

template<class T>
inline T* GameObject::CreateComponentDeferred() {
	if (!T::allowMultipleComponents && HasComponent<T>()) return nullptr;
	if (scene != App->scene->scene) {
		LOG("Deferred component creation is not allowed outside of the main scene.");
		assert(false);
		return nullptr;
	}

	TesseractEvent e(TesseractEventType::ADD_COMPONENT);

	e.variant.emplace<AddComponentStruct>(new T(this, GenerateUID(), active));

	//std::get<AddComponentStruct>(e.variant).component = new T(this, GenerateUID(), active);

	App->events->AddEvent(e);

	return (T*) e.Get<AddComponentStruct>().component;
}

template<class T>
inline bool GameObject::HasComponent() const {
	for (const std::pair<ComponentType, UID>& pair : components) {
		if (pair.first == T::staticType) {
			return true;
		}
	}

	return false;
}

template<class T>
inline T* GameObject::GetComponent() const {
	for (const std::pair<ComponentType, UID>& pair : components) {
		if (pair.first == T::staticType) {
			return (T*) scene->GetComponentByTypeAndId(pair.first, pair.second);
		}
	}
	return nullptr;
}

template<class T>
inline std::vector<T*> GameObject::GetComponents() const {
	std::vector<T*> auxComponents;

	for (const std::pair<ComponentType, UID>& pair : components) {
		if (pair.first == T::staticType) {
			auxComponents.push_back((T*) scene->GetComponentByTypeAndId(pair.first, pair.second));
		}
	}

	return auxComponents;
}

template<class T>
inline GameObject* GameObject::HasComponentInAnyParent(GameObject* current) const {
	T* component = current->GetComponent<T>();
	if (component != nullptr) {
		return current;
	} else {
		if (current->GetParent() != nullptr) {
			return HasComponentInAnyParent<T>(current->GetParent());
		}
	}

	return nullptr;
}
