#include "ComponentAnimation.h"
#include "Resources/GameObject.h"
#include "Resources/AnimationController.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"

#include "Utils/Leaks.h"

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

void ComponentAnimation::Update() {
	animationController->Update();
	OnUpdate();
}
