#include "ExitButton.h"

#include "GameplaySystems.h"

GENERATE_BODY_IMPL(ExitButton);

void ExitButton::Start() {
}

void ExitButton::Update() {
}

void ExitButton::OnButtonClick() {
	SceneManager::ExitGame();
}