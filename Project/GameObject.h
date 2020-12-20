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

	GameObject(const GameObject& other);

	void Init();
	void Update();
	void CleanUp();
	void Enable();
	void Disable();
	bool IsActive() const;

	UID GetID();

	template<class T>
	T* CreateComponent();
	template<class T>
	T* GetComponent() const;
	template<class T>
	std::vector<T*> GetComponents() const;

	void RemoveComponent(Component* component);

	void SetParent(GameObject* game_object);
	GameObject* GetParent() const;

	void AddChild(GameObject* game_object);
	void RemoveChild(GameObject* game_object);
	const std::vector<GameObject*>& GetChildren() const;
	bool IsDescendantOf(GameObject* game_object);

public:
	std::string name = "GameObject";
	UID id = 0;
	std::vector<Component*> components;

private:
	bool active = true;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};

template<class T>
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
