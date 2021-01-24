#pragma once

#include "Components/Component.h"
#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Utils/UID.h"
#include "FileSystem/JsonValue.h"

#include <vector>
#include <string>

class GameObject
{
public:
	void Init();
	void InitComponents();
	void Update();
	void DrawGizmos();
	void CleanUp();
	void Enable();
	void Disable();
	bool IsActive() const;

	UID GetID();

	template<class T> T* CreateComponent(bool active = true);
	template<class T> T* GetComponent() const;
	template<class T> std::vector<T*> GetComponents() const;

	void RemoveComponent(Component* component);

	void SetParent(GameObject* game_object);
	GameObject* GetParent() const;

	void AddChild(GameObject* game_object);
	void RemoveChild(GameObject* game_object);
	const std::vector<GameObject*>& GetChildren() const;
	bool IsDescendantOf(GameObject* game_object);

	void Save(JsonValue j_game_object) const;
	void Load(JsonValue j_game_object);
	void PostLoad(JsonValue j_game_object);

public:
	UID id = 0;
	std::string name = "GameObject";
	std::vector<Component*> components;

	bool is_in_quadtree = false;

	// Auxiliary variable to help with iterating on the Quadtree
	bool flag = false;

private:
	bool active = true;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};

template<class T>
inline T* GameObject::CreateComponent(bool active)
{
	T* component = new T(*this, active);
	components.push_back(component);
	return component;
}

template<class T>
inline T* GameObject::GetComponent() const
{
	for (Component* component : components)
	{
		if (component->GetType() == T::static_type)
		{
			return (T*) component;
		}
	}

	return nullptr;
}

template<class T>
inline std::vector<T*> GameObject::GetComponents() const
{
	std::vector<T*> aux_components;

	for (Component* component : components)
	{
		if (component->GetType() == T::static_type)
		{
			aux_components.push_back((T*) component);
		}
	}

	return aux_components;
}
