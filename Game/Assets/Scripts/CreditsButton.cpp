#include "CreditsButton.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(CreditsButton);

void CreditsButton::Start() {
	Button = GameplaySystems::GetGameObject("Credits");
}

void CreditsButton::Update() {
}

void CreditsButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/CreditsScene.scene");
}