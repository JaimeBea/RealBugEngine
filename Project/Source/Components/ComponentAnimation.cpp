#include "ComponentAnimation.h"

#include "Application.h"
#include "Resources/GameObject.h"
#include "Resources/AnimationController.h"
#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceTransition.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"

#include "Utils/Leaks.h"

#define JSON_TAG_ANIMATION_CONTROLLER "AnimationController"
#define JSON_TAG_RESOURCE_ANIMATION "ResourceAnimation"


void ComponentAnimation::Update() {
	animationController->Update();
	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	ImGui::TextColored(App->editor->titleColor, "Animation");
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	// TODO
	/*JsonValue jAnimationController = jComponent[JSON_TAG_ANIMATION_CONTROLLER];
	jAnimationController = animationController;

	JsonValue jResourceAnimation = jComponent[JSON_TAG_RESOURCE_ANIMATION];
	jResourceAnimation = animationResource;*/
}

void ComponentAnimation::Load(JsonValue jComponent) {
	// TODO
	/*JsonValue jAnimationController = jComponent[JSON_TAG_ANIMATION_CONTROLLER];
	animationController = jAnimationController;

	JsonValue jResourceAnimation = jComponent[JSON_TAG_RESOURCE_ANIMATION];
	animationResource = jResourceAnimation;*/
}

void ComponentAnimation::OnStop() {
	animationController->Stop();
}
void ComponentAnimation::OnPlay() {
	animationController->Play();
}

void ComponentAnimation::OnUpdate() {
	//Update gameobjects matrix
	for (GameObject* child : GetOwner().GetChildren()) {
		if (child->name == "Ctrl_Grp") {
			// One iteration loop
			for (GameObject* child2 : child->GetChildren()) {
				if (child2->name == "Root") {
					UpdateAnimations(child2);
				}
			}
		}
	}
}

void ComponentAnimation::OnTrigger(std::string trigger) {
	ResourceTransition* transition = stateMachineResource->FindTransitionGivenName(trigger);
	if (transition != nullptr) {
		currentTransitions.push_back(transition);
	}
}

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position = float3::zero;
	Quat rotation = Quat::identity;

	for(ResourceTransition* transition : currentTransitions) {
		//interpolateTransition(transition.source, transition.target, postion, rotation);		
	}

	bool result = animationController->GetTransform(gameObject->name.c_str(), position, rotation);

	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();

	if (componentTransform && result) {
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations(child);
	}
}
