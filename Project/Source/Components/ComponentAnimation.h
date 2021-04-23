#pragma once

#include "Component.h"
#include "Application.h"
#include "AnimationController.h"
#include "States.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceStateMachine.h"
#include <string>
#include "Utils/UID.h"
#include "AnimationInterpolation.h"

class GameObject;
class ResourceAnimation;
class ResourceTransition;

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

	States GetCurrentState() {
		return currentState;
	}
	void SetCurrentState(const States& mCurrentState) {
		currentState = mCurrentState;
	}

	States GetInitialState() {
		return initialState;
	}

	void SetInitialState(const States& mInitalState) {
		initialState = mInitalState;
	}

public:
	UID stateMachineResourceUID = 0;
	States currentState;
	States initialState;

private:
	void UpdateAnimations(GameObject *gameObject);

	std::list<AnimationInterpolation> animationInterpolations;
	int t = 0;
};


