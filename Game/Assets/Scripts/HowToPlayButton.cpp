#include "HowToPlayButton.h"

#include "GameplaySystems.h"

GENERATE_BODY_IMPL(HowToPlayButton);

void HowToPlayButton::Start() {
}

void HowToPlayButton::Update() {
}

void HowToPlayButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/HowToPlay.scene");
}