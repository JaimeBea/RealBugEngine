#include "GameController.h"

#include "GameplaySystems.h"

GENERATE_BODY_IMPL(GameController);

void GameController::Start() {
	gameCamera = GameplaySystems::GetGameObject("Game Camera");
	godCamera = GameplaySystems::GetGameObject("God Camera");
	GameplaySystems::SetRenderCamera(gameCamera);
	godCameraActive = false;
	if (gameCamera && godCamera) godModeAvailable = true;
}

void GameController::Update() {
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_G)) {
		if (!godModeAvailable) return;

		if (godCameraActive) {
			GameplaySystems::SetRenderCamera(gameCamera);
		}
		else {
			GameplaySystems::SetRenderCamera(godCamera);
		}
	}
}
