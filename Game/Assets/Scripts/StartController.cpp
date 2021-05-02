#include "StartController.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentCamera.h"

EXPOSE_MEMBERS(StartController) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

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