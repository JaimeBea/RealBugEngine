#include "Component.h"

#include "FileSystem/JsonValue.h"

#include "Utils/Leaks.h"

Component::Component(ComponentType type_, GameObject& owner_, bool active_)
	: type(type_)
	, owner(owner_)
	, active(active_) {}

Component::~Component() {}

void Component::Init() {}

void Component::Update() {}

void Component::DrawGizmos() {}

void Component::OnTransformUpdate() {}

void Component::OnEditorUpdate() {}

void Component::Save(JsonValue jComponent) const {}

void Component::Load(JsonValue jComponent) {}

void Component::DuplicateComponent(GameObject& owner) {}

void Component::Enable() {
	active = true;
}

void Component::Disable() {
	active = false;
}

ComponentType Component::GetType() const {
	return type;
}

GameObject& Component::GetOwner() const {
	return owner;
}

bool Component::IsActive() const {
	return active;
}
