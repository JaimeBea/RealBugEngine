#pragma once

class GameObject;

#define REGISTER_COMPONENT(component_type) static const ComponentType static_type = component_type

enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
	BOUNDING_BOX,
	UNKNOWN
};

class Component
{
public:
	Component(ComponentType type, GameObject& owner, bool active = true);

	virtual void Update();
	virtual void OnEditorUpdate();

	void Enable();
	void Disable();

	ComponentType GetType() const;
	GameObject& GetOwner() const;
	bool IsActive() const;

private:
	const ComponentType type = ComponentType::UNKNOWN;
	GameObject& owner;

	bool active = true;
};
