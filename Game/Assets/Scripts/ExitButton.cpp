#include "ExitButton.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(ExitButton) {

};

GENERATE_BODY_IMPL(ExitButton);

void ExitButton::Start() {
}

void ExitButton::Update() {
}

void ExitButton::OnButtonClick() {
	SceneManager::ExitGame();
}