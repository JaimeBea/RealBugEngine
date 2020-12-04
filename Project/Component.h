#pragma once

class GameObject;

enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	BOUNDING_SPHERE,
	UNKNOWN
};

class Component
{
public:
	Component(ComponentType type, GameObject& owner, bool active = true);

	virtual void Update();

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

