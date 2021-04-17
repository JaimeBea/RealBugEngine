#pragma once

#include "Component.h"
#include "Application.h"
#include "AnimationController.h"
#include "States.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceStateMachine.h"
#include <string>
#include "Utils/UID.h"

class GameObject;
class ResourceAnimation;
class ResourceTransition;
class AnimationInterpolation;

class ComponentAnimation : public Component {
public:
	REGISTER_COMPONENT(ComponentAnimation, ComponentType::ANIMATION, false); // Refer to ComponentType for the Constructor

	void Update() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void OnUpdate();	
	
	void SendTrigger(std::string trigger);

	States* GetCurrentState() {
		return currentState;
	}
	void SetCurrentState(States* mCurrentState) {
		currentState = mCurrentState;
	}

	States* GetInitialState() {
		return initialState;
	}

	void SetInitialState(States* mInitalState) {
		initialState = mInitalState;
	}

public:
	UID stateMachineResourceUID = 0;
	States* currentState = nullptr;
	States* initialState = nullptr;

private:
	void UpdateAnimations(GameObject *gameObject);

	std::list<AnimationInterpolation*> animationInterpolations;
	int t = 0;
};


