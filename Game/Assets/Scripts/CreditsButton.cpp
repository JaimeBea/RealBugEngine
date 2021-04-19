#include "CreditsButton.h"

#include "GameplaySystems.h"

GENERATE_BODY_IMPL(CreditsButton);

void CreditsButton::Start() {
}

void CreditsButton::Update() {
}

void CreditsButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/CreditsScene.scene");
}