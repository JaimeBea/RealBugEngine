#include "GameObject.h"

#include "Globals.h"

#include "Math/myassert.h"

#include "Leaks.h"

GameObject::GameObject()
	: id(GenerateUID()) {}

GameObject::GameObject(UID id_)
	: id(id_) {}

GameObject::GameObject(const GameObject& other)
	: name(other.name)
	, id(GenerateUID())
	, parent(nullptr) {}

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

void GameObject::RemoveComponent(Component* to_remove)
{
	for (Component* component : components)
	{
		if (component == to_remove)
		{
			components.erase(std::remove(components.begin(), components.end(), to_remove), components.end());
		}
	}
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

GameObject* GameObject::GetParent() const
{
	return parent;
}

void GameObject::AddChild(GameObject* game_object)
{
	game_object->SetParent(this);
}

void GameObject::RemoveChild(GameObject* game_object)
{
	game_object->SetParent(nullptr);
}

const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}

bool GameObject::IsDescendantOf(GameObject* game_object)
{
	if (this->GetParent() == nullptr) return false;
	if (this->GetParent() == game_object) return true;
	return this->GetParent()->IsDescendantOf(game_object);
}
