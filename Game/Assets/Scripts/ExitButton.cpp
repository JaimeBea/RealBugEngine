#include "LoseButton.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(LoseButton);

void LoseButton::Start() {
	Button = GameplaySystems::GetGameObject("StartButton");
}

void LoseButton::Update() {
}

void LoseButton::OnButtonClick() {
	SceneManager::SceneLoad("Assets/Scenes/Level1.scene");
}