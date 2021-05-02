#include "CanvasImage.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(CanvasImage) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(CanvasImage);

void CanvasImage::Start() {
	gameObject = GameplaySystems::GetGameObject("Background");

	if (gameObject != nullptr) {
		ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();
		if (transform2D) {
			float2 newSize = float2(Screen::GetScreenWitdh(), Screen::GetScreenHeight());
			transform2D->SetSize(newSize);
		}
	}
}

void CanvasImage::Update() {
	if (gameObject != nullptr) {
		ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();
		if (transform2D) {
			float2 newSize = float2(Screen::GetScreenWitdh(), Screen::GetScreenHeight());
			transform2D->SetSize(newSize);
		}
	}
}