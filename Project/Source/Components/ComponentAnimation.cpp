#include "ComponentAnimation.h"

#include "Application.h"
#include "Transition.h"
#include "AnimationInterpolation.h"
#include "GameObject.h"
#include "AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"

#include "Application.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleInput.h"

#include "Utils/Logging.h"
#include "Utils/Leaks.h"
#include "Utils/UID.h"
#include <algorithm>    // std::find
#define JSON_TAG_LOOP "Controller"
#define JSON_TAG_ANIMATION_ID "AnimationId"
#define JSON_TAG_STATE_MACHINE_ID "StateMachineId"
#define JSON_TAG_INITAL_STATE_ID "InitalState"
#define JSON_TAG_CLIP "Clip"

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
			SendTrigger("s2Ts3");
			t = 2;
			LOG("Transition2");
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_3)) {
		if (t != 3) {
			SendTrigger("s3Ts1");
			t = 3;
			LOG("Transition3");
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_4)) {
		if (t != 4) {
			//SendTrigger("s1Ts2");
			SendTrigger("s2Ts3");
			SendTrigger("s3Ts1");
			t = 4;
			LOG("Transition4");
		}
	}
	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	ImGui::TextColored(App->editor->titleColor, "Animation");
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_STATE_MACHINE_ID] = stateMachineResourceUID;
	jComponent[JSON_TAG_INITAL_STATE_ID] = initialState.id;
}

void ComponentAnimation::Load(JsonValue jComponent) {

	stateMachineResourceUID = jComponent[JSON_TAG_STATE_MACHINE_ID];
	if (stateMachineResourceUID != 0) App->resources->IncreaseReferenceCount(stateMachineResourceUID);

	UID initalStateUid = jComponent[JSON_TAG_INITAL_STATE_ID];

	ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);
	
	for (auto& state : resourceStateMachine->states) {
		if (initalStateUid == state.id){
			initialState = state;
			currentState = state;
			break;
		}
	}
}

void ComponentAnimation::DuplicateComponent(GameObject& owner) {
	/*ComponentAnimation* component = owner.CreateComponent<ComponentAnimation>();
	component->animationController.animationID = animationController.animationID;
	component->animationController.loop = animationController.loop;
	component->animationController.currentTime = animationController.currentTime;*/
}

void ComponentAnimation::OnUpdate() {
	// Update gameobjects matrix
	GameObject* rootBone = GetOwner().GetRootBone();

	UpdateAnimations(rootBone);

	currentState.currentTime += App->time->GetDeltaTime();
}

void ComponentAnimation::SendTrigger(std::string trigger) {
	ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);

	Transition* transition = resourceStateMachine->FindTransitionGivenName(trigger);
	if (transition != nullptr) {
		if (animationInterpolations.size() == 0) {
			animationInterpolations.push_front(AnimationInterpolation(transition->source, currentState.currentTime, 0, transition->interpolationDuration));
		}
		animationInterpolations.push_front(AnimationInterpolation(transition->target, 0, 0, transition->interpolationDuration));
	}
}

struct CheckFinishInterpolation {
	bool operator()(AnimationInterpolation& animationInterpolation) {
		if (animationInterpolation.fadeTime >= 0.9) {
			ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);
			componentAnimation->currentState = animationInterpolation.state;
			componentAnimation->currentState.currentTime = animationInterpolation.TransistionTime;
			LOG("CheckFinishInterpolation");
		}
		return animationInterpolation.fadeTime >= 0.9;
	}
	UID stateMachineResourceUID;
	ComponentAnimation *componentAnimation;
};

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position, totalPostion = float3::zero;
	Quat rotation, totalRotation = Quat::identity;

	bool result = true;
	ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);

	if (animationInterpolations.size() > 1) {
		result = AnimationController::InterpolateTransitions(animationInterpolations.begin(), animationInterpolations, *GetOwner().GetRootBone(), *gameObject, position, rotation);
		CheckFinishInterpolation checkFinishInterpolation;
		checkFinishInterpolation.stateMachineResourceUID = stateMachineResourceUID;
		checkFinishInterpolation.componentAnimation = this;
		animationInterpolations.remove_if(checkFinishInterpolation);
		if (animationInterpolations.size() <= 1) {
			animationInterpolations.clear();
		}
	} else {
		ResourceClip* clip  = (ResourceClip*) App->resources->GetResource(currentState.clipUid);
		result = AnimationController::GetTransform(*clip, currentState.currentTime, gameObject->name.c_str(), position, rotation);
	}

	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();

	if (componentTransform && result) {
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations(child);
	}

}
