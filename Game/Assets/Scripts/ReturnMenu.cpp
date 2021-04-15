#include "ReturnMenu.h"

#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(ReturnMenu);

void ReturnMenu::Start() {
	button = GameplaySystems::GetGameObject("ExitButton");
	ComponentTransform2D* transform2D = button->GetComponent<ComponentTransform2D>();
	if (transform2D) {
		float2 buttonSize = transform2D->GetSize();
		float2 newSize = float2(Screen::GetScreenWitdh(), Screen::GetScreenHeight());
		transform2D->SetPosition(float3(newSize.x / 2 - buttonSize.x - padding, newSize.y / 2 - buttonSize.y - padding, transform2D->GetPosition().z));
	}
}

void ReturnMenu::Update() {
	ComponentTransform2D* transform2D = button->GetComponent<ComponentTransform2D>();
	if (transform2D) {
		float2 buttonSize = transform2D->GetSize();
		float2 newSize = float2(Screen::GetScreenWitdh(), Screen::GetScreenHeight());
		transform2D->SetPosition(float3(newSize.x / 2 - buttonSize.x - padding, newSize.y / 2 - buttonSize.y - padding, transform2D->GetPosition().z));
	}
}

void ReturnMenu::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/StartScene.scene");
}