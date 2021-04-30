#include "CanvasImage.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(CanvasImage) {

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