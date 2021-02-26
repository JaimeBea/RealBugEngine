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
			UpdateAnimations(child);
		}
	}
}

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}
	//find gameobject in hash
	float3 position;
	Quat rotation;
	bool result = animationController->GetTransform(gameObject->name.c_str(), position, rotation);
	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();
	if (componentTransform && result) {
		float3 parentPosition = float3::zero;
		Quat parentRotation = Quat::identity;
		
		if (gameObject->GetParent()) {
			ComponentTransform* parentComponentTransform = gameObject->GetParent()->GetComponent<ComponentTransform>();
			if (parentComponentTransform) {
				parentPosition = parentComponentTransform->GetPosition();
				parentRotation = parentComponentTransform->GetRotation();
			}
		}
		
		componentTransform->SetPosition(parentPosition + position);
		componentTransform->SetRotation(parentRotation * rotation);
	}
	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations( child );
	}
}

void ComponentAnimation::Update() {
	OnUpdate();
}
