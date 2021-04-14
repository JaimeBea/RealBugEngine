#include "ExitButton.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(ExitButton);

void ExitButton::Start() {
	Button = GameplaySystems::GetGameObject("ExitButton");
}

void ExitButton::Update() {
}

void ExitButton::OnButtonClick() {
	SceneManager::ExitGame();
}