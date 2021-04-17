#pragma once

#include "Component.h"
#include "Application.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceStates.h"
#include <string>
#include "AnimationController.h"
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

	ResourceStates* GetCurrentState() {
		return currentState;
	}
	void SetCurrentState(ResourceStates* mCurrentState) {
		currentState = mCurrentState;
	}

	ResourceStates* GetInitialState() {
		return initialState;
	}

	void SetInitialState(ResourceStates* mInitalState) {
		initialState = mInitalState;
	}

public:
	UID stateMachineResourceUID = 0;
	ResourceStates* currentState = nullptr;
	ResourceStates* initialState = nullptr;

private:
	void UpdateAnimations(GameObject *gameObject);

	std::list<AnimationInterpolation*> animationInterpolations;
	int t = 0;
};


