#include "StartButton.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
	Button = GameplaySystems::GetGameObject("StartButton");
}

void StartButton::Update() {
}

void StartButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/Level1.scene");
}