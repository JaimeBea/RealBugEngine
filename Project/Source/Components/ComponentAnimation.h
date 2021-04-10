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
	
	AnimationController* animationController = nullptr;
	UID stateMachineResourceUID = 0;

	void SendTrigger(std::string trigger);

	ResourceStates* GetCurrentState() {
		if (stateMachineResourceUID == 0) {
			return nullptr;
		}
		ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*)App->resources->GetResource(stateMachineResourceUID);
		return resourceStateMachine->GetCurrentState();
	}

private:
	void UpdateAnimations(GameObject *gameObject);
	std::string _oldState = "";

	std::list<AnimationInterpolation*> animationInterpolations;
	int t = 0;
};


