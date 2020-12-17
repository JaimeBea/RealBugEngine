#include "Component.h"

Component::Component(ComponentType type_, GameObject& owner_, bool active_) : type(type_), owner(owner_), active(active_) {}

void Component::Update() {}

void Component::OnEditor() {} 

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
