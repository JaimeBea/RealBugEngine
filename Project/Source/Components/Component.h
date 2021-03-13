#pragma once

#include "ComponentType.h"

class JsonValue;
class GameObject;

class Component {
public:
	Component(ComponentType type, GameObject& owner, bool active);

	virtual void Init();
	virtual void Update();
	virtual void DrawGizmos();
	virtual void OnTransformUpdate();
	virtual void OnEditorUpdate();
	virtual void Save(JsonValue jComponent) const;
	virtual void Load(JsonValue jComponent);

	void Enable();
	void Disable();

	ComponentType GetType() const;
	GameObject& GetOwner() const;
	bool IsActive() const;

private:
	ComponentType type = ComponentType::UNKNOWN;

	bool active = true;

protected:
	GameObject& owner;

};
