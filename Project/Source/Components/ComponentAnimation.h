#pragma once

#include "Application.h"
#include "Component.h"
#include "AnimationController.h"
#include "AnimationInterpolation.h"
#include "State.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceStateMachine.h"
#include "Utils/UID.h"
#include <string>

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

	void SendTrigger(const std::string& trigger); // Method to trigger the change of state

	State* GetCurrentState() {
		return currentState;
	}
	void SetCurrentState(State* mCurrentState) {
		currentState = mCurrentState;
	}

	State* GetInitialState() {
		return initialState;
	}

	void SetInitialState(State* mInitalState) {
		initialState = mInitalState;
	}

public:
	UID stateMachineResourceUID = 0;
	State* currentState = nullptr;
	State* initialState = nullptr;

private:
	void UpdateAnimations(GameObject* gameObject);

private:
	std::list<AnimationInterpolation> animationInterpolations; //List of the current interpolations between states
	int t = 0;												   // TODO remove, HardCoded for test transitions
};
