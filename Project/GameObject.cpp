#include "GameObject.h"

#include "Globals.h"

#include "Math/myassert.h"

#include "Leaks.h"

GameObject::GameObject() : id(GenerateUID()) {}

GameObject::GameObject(UID id_) : id(id_) {}

void GameObject::Init() {}

void GameObject::Update()
{
	for (Component* component : components)
	{
		component->Update();
	}

	for (GameObject* child : children)
	{
		child->Update();
	}
}

void GameObject::CleanUp()
{
	SetParent(nullptr);
	while (!children.empty())
	{
		children.back()->CleanUp();
	}
	children.clear();
	for (Component* component : components)
	{
		delete component;
	}
	components.clear();
}

void GameObject::Enable()
{
	active = true;
}

void GameObject::Disable()
{
	active = false;
}

bool GameObject::IsActive() const
{
	return active;
}

UID GameObject::GetID()
{
	return id;
}

void GameObject::SetParent(GameObject* game_object)
{
	if (parent != nullptr)
	{
		bool found = false;
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
		{
			if (*it == this)
			{
				found = true;
				parent->children.erase(it);
				break;
			}
		}
		assert(found);
	}
	parent = game_object;
	if (game_object != nullptr)
	{
		game_object->children.push_back(this);
	}
}

void GameObject::AddChild(GameObject* game_object)
{
	game_object->SetParent(this);
}

void GameObject::RemoveChild(GameObject* game_object)
{
	game_object->SetParent(nullptr);
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}
