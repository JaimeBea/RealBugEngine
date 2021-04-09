#pragma once

#include "Component.h"
#include "AnimationController.h"

class GameObject;

class ComponentAnimation : public Component {
public:
	REGISTER_COMPONENT(ComponentAnimation, ComponentType::ANIMATION, false); // Refer to ComponentType for the Constructor

	void Update() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void OnUpdate();

private:
	void UpdateAnimations(GameObject* gameObject);

public:
	AnimationController animationController;
	// TODO: State machine
};
