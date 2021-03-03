#pragma once

#include "Components/Component.h"
#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Utils/UID.h"
#include "FileSystem/JsonValue.h"

#include <vector>
#include <string>

class GameObject {
public:
	void Init();
	void InitComponents();
	void Update();
	void DrawGizmos();
	void Enable();
	void Disable();
	bool IsActive() const;
	bool IsActiveInHierarchy() const;
	UID GetID();

	template<class T> T* CreateComponent(bool active = true);
	template<class T> T* GetComponent() const;
	template<class T> std::vector<T*> GetComponents() const;

	void RemoveComponent(Component* component);

	void SetParent(GameObject* gameObject);
	GameObject* GetParent() const;

	void AddChild(GameObject* gameObject);
	void RemoveChild(GameObject* gameObject);
	const std::vector<GameObject*>& GetChildren() const;
	bool IsDescendantOf(GameObject* gameObject);
	bool AddComponent(ComponentType type);		 // Functionality for the Add Component button in the inspector.
	bool HasComponent(ComponentType type) const; // Checks if this GameObject has a Component of type 'type'
	bool HasChildren() const;

	void Save(JsonValue jGameObject) const;
	void Load(JsonValue jGameObject);
	void PostLoad(JsonValue jGameObject);

public:
	UID id = 0;
	std::string name = "GameObject";
	std::vector<Component*> components;

	bool isInQuadtree = false;

	// Auxiliary variable to help with iterating on the Quadtree
	bool flag = false;

private:
	bool active = true;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};

template<class T>
inline T* GameObject::CreateComponent(bool active) {
	T* component = new T(*this, active);
	components.push_back(component);
	return component;
}

template<class T>
inline T* GameObject::GetComponent() const {
	for (Component* component : components) {
		if (component->GetType() == T::staticType) {
			return (T*) component;
		}
	}
	return nullptr;
}

template<class T>
inline std::vector<T*> GameObject::GetComponents() const {
	std::vector<T*> auxComponents;

	for (Component* component : components) {
		if (component->GetType() == T::staticType) {
			auxComponents.push_back((T*) component);
		}
	}

	return auxComponents;
}
