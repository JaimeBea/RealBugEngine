#include "ComponentAnimation.h"

#include "Application.h"
#include "Resources/GameObject.h"
#include "Resources/AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "FileSystem/AnimationControllerImporter.h"
#include "FileSystem/AnimationImporter.h"

#include "Utils/Leaks.h"

#define JSON_TAG_CONTROLLER "Controller"

void ComponentAnimation::Update() {
	animationController->Update();
	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	ImGui::TextColored(App->editor->titleColor, "Animation");
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_CONTROLLER] = (animationController != nullptr) ? animationController->fileName.c_str() : "";

	if (animationController != nullptr) {
		AnimationControllerImporter::SaveAnimationController(animationController);
	}
}

void ComponentAnimation::Load(JsonValue jComponent) {
	std::string controllerName = jComponent[JSON_TAG_CONTROLLER];

	if (controllerName != "") {
		if (animationController == nullptr) {
			animationController = App->resources->ObtainAnimationController();
		}
		animationController->fileName = controllerName;
		AnimationControllerImporter::LoadAnimationController(animationController);
	}
}

void ComponentAnimation::OnStop() {
	animationController->Stop();
}

void ComponentAnimation::OnPlay() {
	animationController->Play();
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