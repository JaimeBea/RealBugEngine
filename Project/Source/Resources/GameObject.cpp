#include "GameObject.h"

#include "Globals.h"
#include "Components/ComponentType.h"

#include "Math/myassert.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

#define JSON_TAG_ID "Id"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_ACTIVE "Active"
#define JSON_TAG_PARENT_ID "ParentId"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_COMPONENTS "Components"

void GameObject::Init() {
	id = GenerateUID();
}

void GameObject::InitComponents() {
	for (Component* component : components) {
		component->Init();
	}
}

void GameObject::Update() {
	for (Component* component : components) {
		component->Update();
	}

	for (GameObject* child : children) {
		child->Update();
	}
}

void GameObject::DrawGizmos() {
	for (Component* component : components) {
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

UID GameObject::GetID() {
	return id;
}

void GameObject::RemoveComponent(Component* to_remove) {
	for (Component* component : components) {
		if (component == to_remove) {
			components.erase(std::remove(components.begin(), components.end(), to_remove), components.end());
		}
	}
}

void GameObject::SetParent(GameObject* game_object) {
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
	parent = game_object;
	if (game_object != nullptr) {
		game_object->children.push_back(this);
	}
}

GameObject* GameObject::GetParent() const {
	return parent;
}

void GameObject::AddChild(GameObject* game_object) {
	game_object->SetParent(this);
}

void GameObject::RemoveChild(GameObject* game_object) {
	game_object->SetParent(nullptr);
}

const std::vector<GameObject*>& GameObject::GetChildren() const {
	return children;
}

bool GameObject::IsDescendantOf(GameObject* game_object) {
	if (GetParent() == nullptr) return false;
	if (GetParent() == game_object) return true;
	return GetParent()->IsDescendantOf(game_object);
}

void GameObject::Save(JsonValue j_game_object) const {
	j_game_object[JSON_TAG_ID] = id;
	j_game_object[JSON_TAG_NAME] = name.c_str();
	j_game_object[JSON_TAG_ACTIVE] = active;
	j_game_object[JSON_TAG_PARENT_ID] = parent != nullptr ? parent->id : 0;

	JsonValue j_components = j_game_object[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < components.size(); ++i) {
		JsonValue j_component = j_components[i];
		Component& component = *components[i];

		j_component[JSON_TAG_TYPE] = (unsigned) component.GetType();
		j_component[JSON_TAG_ACTIVE] = component.IsActive();
		component.Save(j_component);
	}
}

void GameObject::Load(JsonValue j_game_object) {
	id = j_game_object[JSON_TAG_ID];
	name = j_game_object[JSON_TAG_NAME];
	active = j_game_object[JSON_TAG_ACTIVE];

	JsonValue j_components = j_game_object[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < j_components.Size(); ++i) {
		JsonValue j_component = j_components[i];

		ComponentType type = (ComponentType)(unsigned) j_component[JSON_TAG_TYPE];
		bool active = j_component[JSON_TAG_ACTIVE];

		Component* component = CreateComponentByType(*this, type);
		component->Load(j_component);
	}

	is_in_quadtree = false;
}

void GameObject::PostLoad(JsonValue j_game_object) {
	UID parent_id = j_game_object[JSON_TAG_PARENT_ID];
	GameObject* parent = App->scene->GetGameObject(parent_id);
	SetParent(parent);
}
