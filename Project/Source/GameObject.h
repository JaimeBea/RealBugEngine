#pragma once

#include "Components/Component.h"
#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Utils/Logging.h"
#include "Utils/UID.h"
#include "FileSystem/JsonValue.h"

#include <vector>
#include <string>

class GameObject {
public:
	void InitComponents();
	void Update();
	void DrawGizmos();

	void Enable();
	void Disable();
	bool IsActive() const;
	bool IsActiveInHierarchy() const;

	UID GetID() const;

	template<class T> T* CreateComponent(bool active = true);
	template<class T> bool HasComponent() const;
	template<class T> T* GetComponent() const;
	template<class T> std::vector<T*> GetComponents() const;
	template<class T> GameObject* HasComponentInAnyParent(GameObject* current) const;		// Finds in the current object or in any parent of this Object the T Component. Returns the GameObject if found, else nullptr
	std::vector<Component*> GetComponents() const;
	void RemoveComponent(Component* component);
	void RemoveAllComponents();

	void SetParent(GameObject* gameObject);
	GameObject* GetParent() const;

	void AddChild(GameObject* gameObject);
	void RemoveChild(GameObject* gameObject);
	const std::vector<GameObject*>& GetChildren() const;
	bool IsDescendantOf(GameObject* gameObject);
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

private:
	Component* GetComponentByTypeAndId(ComponentType type, UID componentId) const;
	Component* CreateComponentByTypeAndId(ComponentType type, UID componentId);
	void RemoveComponentByTypeAndId(ComponentType type, UID componentId);

private:
	bool active = true;
	GameObject* parent = nullptr;
	std::vector<std::pair<ComponentType, UID>> components;
	std::vector<GameObject*> children;
};

template<class T>
inline T* GameObject::CreateComponent(bool active) {
	if (!T::allowMultipleComponents && HasComponent<T>()) return nullptr;
	UID componentId = GenerateUID();
	components.push_back(std::pair<ComponentType, UID>(T::staticType, componentId));
	return (T*) CreateComponentByTypeAndId(T::staticType, componentId);
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
			return (T*) GetComponentByTypeAndId(pair.first, pair.second);
		}
	}
	return nullptr;
}

template<class T>
inline std::vector<T*> GameObject::GetComponents() const {
	std::vector<T*> auxComponents;

	for (const std::pair<ComponentType, UID>& pair : components) {
		if (pair.first == T::staticType) {
			auxComponents.push_back((T*) GetComponentByTypeAndId(pair.first, pair.second));
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
