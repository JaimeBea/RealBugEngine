#include "ComponentAnimation.h"

#include "Application.h"
#include "Transition.h"
#include "GameObject.h"
#include "AnimationInterpolation.h"
#include "AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceClip.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleInput.h"
#include "Utils/UID.h"
#include "Utils/Logging.h"

#include <algorithm> // std::find

#include "Utils/Leaks.h"

#define JSON_TAG_LOOP "Controller"
#define JSON_TAG_ANIMATION_ID "AnimationId"
#define JSON_TAG_STATE_MACHINE_ID "StateMachineId"
#define JSON_TAG_INITAL_STATE_ID "InitalState"
#define JSON_TAG_CLIP "Clip"

void ComponentAnimation::Update() {
	// TODO remove, HardCoded for test transitions
	/*
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
			SendTrigger("s1Ts2");
			SendTrigger("s2Ts3");
			SendTrigger("s3Ts1");
			t = 4;
			LOG("Transition4");
		}
	}*/

	if (!initialState) { //Checking if there is no state machine
		LoadResourceStateMachine();
	}

	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	ImGui::TextColored(App->editor->titleColor, "Animation");
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_STATE_MACHINE_ID] = stateMachineResourceUID;
	jComponent[JSON_TAG_INITAL_STATE_ID] = initialState->id;
}

void ComponentAnimation::Load(JsonValue jComponent) {
	stateMachineResourceUID = jComponent[JSON_TAG_STATE_MACHINE_ID];
	if (stateMachineResourceUID != 0) App->resources->IncreaseReferenceCount(stateMachineResourceUID);

	initalStateUid = jComponent[JSON_TAG_INITAL_STATE_ID];
	LoadResourceStateMachine();
}

void ComponentAnimation::DuplicateComponent(GameObject& owner) {
	// TODO
	/*ComponentAnimation* component = owner.CreateComponent<ComponentAnimation>();
	component->stateMachineResourceUID = stateMachineResourceUID;
	component->currentState = currentState;
	component->initialState = initialState;*/
}

void ComponentAnimation::OnUpdate() {
	// Update gameobjects matrix
	GameObject* rootBone = GetOwner().GetRootBone();

	UpdateAnimations(rootBone);

	ResourceClip* currentClip = App->resources->GetResource<ResourceClip>(currentState->clipUid);
	currentState->currentTime += App->time->GetDeltaTime() * currentClip->speed;
}

void ComponentAnimation::SendTrigger(const std::string& trigger) {
	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUID);

	Transition* transition = resourceStateMachine->FindTransitionGivenName(trigger);
	if (transition != nullptr) {
		if (animationInterpolations.size() == 0) {
			animationInterpolations.push_front(AnimationInterpolation(&transition->source, currentState->currentTime, 0, transition->interpolationDuration));
		}
		animationInterpolations.push_front(AnimationInterpolation(&transition->target, 0, 0, transition->interpolationDuration));
	}
}

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position = float3::zero;
	Quat rotation = Quat::identity;

	bool result = true;
	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUID);
	if (!resourceStateMachine) {
		return;
	}

	if (animationInterpolations.size() > 1) {
		result = AnimationController::InterpolateTransitions(animationInterpolations.begin(), animationInterpolations, *GetOwner().GetRootBone(), *gameObject, position, rotation);

		//Updating times
		if (gameObject == GetOwner().GetRootBone()) { // Only udate currentTime for the rootBone
			State* newState = AnimationController::UpdateTransitions(animationInterpolations, App->time->GetDeltaTime());
			if (newState) {
				currentState = newState;
			}
		}

	} else {
		ResourceClip* clip = App->resources->GetResource<ResourceClip>(currentState->clipUid);
		result = AnimationController::GetTransform(*clip, currentState->currentTime, gameObject->name.c_str(), position, rotation);
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

void ComponentAnimation::LoadResourceStateMachine() {
	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUID);

	if (resourceStateMachine) {
		for (auto& state : resourceStateMachine->states) {
			if (initalStateUid == state.id) {
				initialState = &state;
				currentState = &state;
				break;
			}
		}
	}
}
