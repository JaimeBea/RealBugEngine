#include "Component.h"

#include "JsonValue.h"

#include "Leaks.h"

Component::Component(ComponentType type_, GameObject& owner_, bool active_)
	: type(type_)
	, owner(owner_)
	, active(active_) {}

void Component::Init() {}

void Component::Update() {}

void Component::DrawGizmos() {}

void Component::OnTransformUpdate() {}

void Component::OnEditorUpdate() {}

void Component::Save(JsonValue j_component) const {}

void Component::Load(JsonValue j_component) {}

void Component::Enable()
{
	active = true;
}

void Component::Disable()
{
	active = false;
}

ComponentType Component::GetType() const
{
	return type;
}

GameObject& Component::GetOwner() const
{
	return owner;
}

bool Component::IsActive() const
{
	return active;
}
