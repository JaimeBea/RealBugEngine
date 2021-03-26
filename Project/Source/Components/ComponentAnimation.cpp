#include "ComponentAnimation.h"

#include "Application.h"
#include "Resources/GameObject.h"
#include "Resources/AnimationController.h"
//#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceTransition.h"
#include "Resources/AnimationInterpolation.h"
//#include "Resources/ResourceStates.h"
#include "Resources/Clip.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"

#include "Application.h"
#include "Modules/ModuleInput.h"

#include "Utils/Logging.h"
#include "Utils/Leaks.h"

#define JSON_TAG_ANIMATION_CONTROLLER "AnimationController"
#define JSON_TAG_RESOURCE_ANIMATION "ResourceAnimation"


void ComponentAnimation::Update() {
	if (App->input->GetKey(SDL_SCANCODE_1)) {
		if (t != 1) {
			SendTrigger("s1Ts2");
			t = 1;
			LOG("Transition1");
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_2)) {
		if (t != 2) {
			SendTrigger("s2Ts1");
			t = 2;
			LOG("Transition2");
		}
	}

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

void ComponentAnimation::SendTrigger(std::string trigger) {
	ResourceTransition* transition = stateMachineResource->GetValidTransition(trigger);
	if (transition != nullptr) {
		animationInterpolations.push_front(new AnimationInterpolation(transition->source, 0, 0, transition->interpolationDuration));
		animationInterpolations.push_front(new AnimationInterpolation(transition->target, 0, 0, transition->interpolationDuration));
		animationController->currentTime = 0;
	}
}

struct CheckFinishInterpolation {
	bool operator()(AnimationInterpolation*& animationInterpolation) {
		if (animationInterpolation->fadeTime >= 0.9) {
			stateMachine->SetCurrentState(animationInterpolation->state);
			//animationController->currentTime = 0;
			LOG("CheckFinishInterpolation");
		}
		return animationInterpolation->fadeTime >= 0.9;
	}
	ResourceStateMachine* stateMachine;
	AnimationController* animationController;
};

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position, totalPostion = float3::zero;
	Quat rotation, totalRotation = Quat::identity;

	bool result = true;
	std::vector<ResourceTransition*> toDelete;

	if (animationInterpolations.size() > 1) {
		result = animationController->InterpolateTransitions(animationInterpolations.begin(), animationInterpolations, gameObject, position, rotation);
		CheckFinishInterpolation checkFinishInterpolation;
		checkFinishInterpolation.stateMachine = stateMachineResource;
		checkFinishInterpolation.animationController = animationController;
		animationInterpolations.remove_if(checkFinishInterpolation);
		if (animationInterpolations.size() <= 1) {
			animationInterpolations.clear();
		}
	} else {
		result = animationController->GetTransform(stateMachineResource->GetCurrentState()->clip, gameObject->name.c_str(), position, rotation);
	}

	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();

	if (componentTransform && result) {
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations(child);
	}

	animationController->SetAnimationResource(stateMachineResource->GetCurrentState()->clip->animation); // Update current animation resource

}
