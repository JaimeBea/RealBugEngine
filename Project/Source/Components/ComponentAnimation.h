#pragma once
#include "Component.h"
#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceStates.h"
#include <string>
#include "AnimationController.h"

class GameObject;
class ResourceAnimation;
class ResourceTransition;
class AnimationInterpolation;

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

	void OnUpdate();
	
	AnimationController* animationController = nullptr;
	ResourceStateMachine* stateMachineResource = nullptr;

	void SendTrigger(std::string trigger);

	ResourceStates* GetCurrentState() {
		if (stateMachineResource == nullptr) {
			return nullptr;
		}

		return stateMachineResource->GetCurrentState();
	}

private:
	void UpdateAnimations(GameObject *gameObject);
	std::string _oldState = "";

	std::list<AnimationInterpolation*> animationInterpolations;
	int t = 0;
};


