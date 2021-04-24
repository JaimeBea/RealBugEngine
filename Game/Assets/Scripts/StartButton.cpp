#include "StartButton.h"

#include "GameplaySystems.h"

GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
}

void StartButton::Update() {
}

void StartButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/Level1.scene");
}