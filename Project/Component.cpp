#include "Component.h"

Component::Component(ComponentType type_, GameObject& owner_, bool active_)
	: type(type_)
	, owner(owner_)
	, active(active_) {}

void Component::Init() {}

void Component::Update() {}

void Component::DrawDebugDraw() {}

void Component::OnEditorUpdate() {}

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
