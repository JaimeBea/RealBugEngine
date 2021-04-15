#include "StartController.h"

#include "GameplaySystems.h"

GENERATE_BODY_IMPL(StartController);

void StartController::Start() {
	GameObject* gameCamera = GameplaySystems::GetGameObject("Game Camera");
	if (gameCamera) GameplaySystems::SetRenderCamera(gameCamera);
}

void StartController::Update() {
}