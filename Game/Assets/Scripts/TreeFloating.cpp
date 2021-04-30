#include "TreeFloating.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include <cmath>

EXPOSE_MEMBERS(TreeFloating) {

};

GENERATE_BODY_IMPL(TreeFloating);

void TreeFloating::Start() {
	offset = 0.f;
	gameObject = GameplaySystems::GetGameObject("Trees");
	if (gameObject) transform = gameObject->GetComponent<ComponentTransform>();
	if (transform) initialPosition = transform->GetPosition();
	

}

void TreeFloating::Update() {
	
	if (gameObject) transform = gameObject->GetComponent<ComponentTransform>();
	if (!transform) return;
	offset = offset + Time::GetDeltaTime();
	transform->SetPosition(initialPosition + float3(0.f, Sin(offset), 0.f));
}