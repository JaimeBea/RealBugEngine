#include "HowToPlayButton.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(HowToPlayButton);

void HowToPlayButton::Start() {
	Button = GameplaySystems::GetGameObject("HowToPlay");
}

void HowToPlayButton::Update() {
}

void HowToPlayButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/HowToPlay.scene");
}