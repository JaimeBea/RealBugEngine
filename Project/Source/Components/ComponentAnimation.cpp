#include "ComponentAnimation.h"
#include "Resources/GameObject.h"
#include "Resources/AnimationController.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
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
	float3 scale = float3::one;
	bool result = animationController->GetTransform(gameObject->name.c_str(), position, rotation);
	if (gameObject->name == "Hips") {
		int aux = 0;
	}
	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();
	if (componentTransform && result) {
		if (!position.Equals(float3::zero)) {
			componentTransform->SetPosition(position);
		}
		if (!rotation.Equals(Quat::identity)) {
			componentTransform->SetRotation(rotation);
		}
	}
	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations( child );
	}
}

void ComponentAnimation::Update() {
	OnUpdate();
	animationController->Update();
}
