#include "TreeFloating.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include <cmath>

GENERATE_BODY_IMPL(TreeFloating);

void TreeFloating::Start() {
	offset = 0.f;
	gameObject = GameplaySystems::GetGameObject("Trees");
	if(gameObject) transform = gameObject->GetComponent<ComponentTransform>();
}

void TreeFloating::Update() {
	if (!transform) return;
	offset = offset+Time::GetDeltaTime();
	transform->SetPosition(transform->GetPosition() + float3(0.f, Sin(offset)*0.02, 0.f));
}