#include "TreeFloating.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include <cmath>

EXPOSE_MEMBERS(TreeFloating) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
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