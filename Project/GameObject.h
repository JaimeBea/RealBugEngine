#pragma once

#include "Application.h"
#include "ModuleScene.h"
#include "Component.h"
#include "UID.h"

#include <vector>
#include <string>

class GameObject
{
public:
	GameObject();
	GameObject(UID id);

	void Init();
	void Update();
	void CleanUp();

	UID GetID();

	template <class T> T* CreateComponent();
	template <class T> T* GetComponent() const;
	template <class T> std::vector<T*> GetComponents() const;

	void SetParent(GameObject* game_object);
	GameObject* GetParent() const;

	void AddChild(GameObject* game_object);
	void RemoveChild(GameObject* game_object);
	const std::vector<GameObject*>& GetChildren() const;

public:
	std::string name = "GameObject";

private:
	UID id = 0;

	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};

template <class T>
inline T* GameObject::CreateComponent()
{
	T* component = new T(*this);
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
			return (T*)component;
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
			aux_components.push_back((T*)component);
		}
	}

	return aux_components;
}
