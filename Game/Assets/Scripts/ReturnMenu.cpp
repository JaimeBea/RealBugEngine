#include "ReturnMenu.h"

#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(ReturnMenu);

void ReturnMenu::Start() {
	button = GameplaySystems::GetGameObject("ExitButton");
}

void ReturnMenu::Update() {
	ComponentTransform2D* transform = button->GetComponent<ComponentTransform2D>();
	float2 buttonSize = transform->GetSize();
	float2 newSize = float2(Screen::GetScreenWitdh(), Screen::GetScreenHeight());
	transform->SetPosition(float3(newSize.x / 2 - buttonSize.x - padding, newSize.y / 2 - buttonSize.y - padding, transform->GetPosition().z));
}

void ReturnMenu::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/StartScene.scene");
}