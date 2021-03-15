#pragma once
#include "Component.h"

class GameObject;
class AnimationController;
class ResourceAnimation;
class ResourceStateMachine;
class ResourceTransition;

#include <string>


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

	void OnTrigger(std::string trigger);

	AnimationController* animationController;
	ResourceAnimation* animationResource;
	ResourceStateMachine* stateMachineResource = nullptr;

private:
	void UpdateAnimations(GameObject *gameObject);

	std::vector<ResourceTransition*> currentTransitions;
};
