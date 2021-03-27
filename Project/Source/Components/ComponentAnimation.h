#pragma once
#include "Component.h"
#include "AnimationController.h"

class GameObject;
class ResourceAnimation;

class ComponentAnimation : public Component {
public:
	REGISTER_COMPONENT(ComponentAnimation, ComponentType::ANIMATION, false); // Refer to ComponentType for the Constructor

	//void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	//void DrawGizmos() override;
	//void OnTransformUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void OnStop();
	void OnPlay();
	void OnUpdate();

private:
	void UpdateAnimations(GameObject* gameObject);

public:
	AnimationController animationController;
	// State machine
};
