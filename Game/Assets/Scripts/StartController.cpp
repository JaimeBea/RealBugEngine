#include "StartController.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentCamera.h"

GENERATE_BODY_IMPL(StartController);

void StartController::Start() {
	GameObject* gameCamera = GameplaySystems::GetGameObject("Game Camera");
	if (gameCamera) {
		ComponentCamera* camera = gameCamera->GetComponent<ComponentCamera>();
		if(camera) GameplaySystems::SetRenderCamera(camera);
	}
	
}

void StartController::Update() {
}