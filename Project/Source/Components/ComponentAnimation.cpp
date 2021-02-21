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
	//UpdateAnimations(&GetOwner());
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
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}
	for (GameObject* child : GetOwner().GetChildren()) {
		UpdateAnimations( child );
	}
}

void ComponentAnimation::Update() {
	OnUpdate();
}
