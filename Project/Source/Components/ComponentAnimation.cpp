#include "ComponentAnimation.h"

#include "Application.h"
#include "GameObject.h"
#include "AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"

#include "Utils/Leaks.h"

#define JSON_TAG_LOOP "Controller"

void ComponentAnimation::Update() {
	animationController.Update();
	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	ImGui::TextColored(App->editor->titleColor, "Animation");
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	// TODO: Save state machine resource UID
	jComponent[JSON_TAG_LOOP] = animationController.loop;
}

void ComponentAnimation::Load(JsonValue jComponent) {
	animationController.loop = jComponent[JSON_TAG_LOOP];
}

void ComponentAnimation::DuplicateComponent(GameObject& owner) {
	ComponentAnimation* component = owner.CreateComponent<ComponentAnimation>();
	component->animationController.animationID = animationController.animationID;
	component->animationController.loop = animationController.loop;
	component->animationController.running = animationController.running;
	component->animationController.currentTime = animationController.currentTime;
}

void ComponentAnimation::OnStop() {
	animationController.Stop();
}

void ComponentAnimation::OnPlay() {
	animationController.Play();
}

void ComponentAnimation::OnUpdate() {
	// Update gameobjects matrix

	GameObject* rootBone = GetOwner().GetRootBone();

	for (GameObject* child : rootBone->GetChildren()) {
		UpdateAnimations(child);
	}
}

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position = float3::zero;
	Quat rotation = Quat::identity;

	bool result = animationController.GetTransform(gameObject->name.c_str(), position, rotation);

	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();

	if (componentTransform && result) {
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations(child);
	}
}