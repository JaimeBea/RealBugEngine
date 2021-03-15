#pragma once
#include "Component.h"
#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceStates.h"
#include <string>
class GameObject;
class AnimationController;
class ResourceAnimation;
class ResourceTransition;



class ComponentAnimation : public Component {
public:
	REGISTER_COMPONENT(ComponentAnimation, ComponentType::ANIMATION); // Refer to ComponentType for the Constructor

	//void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	//void DrawGizmos() override; 
	//void OnTransformUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override; 

	void OnStop();
	void OnPlay();
	void OnUpdate();

	AnimationController* animationController;
	ResourceAnimation* animationResource;
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

	std::vector<ResourceTransition*> currentTransitions;

};
