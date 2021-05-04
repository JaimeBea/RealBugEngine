#include "WinLose.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(WinLose) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(WinLose);

void WinLose::Start() {
	winCon = GameplaySystems::GetGameObject("WinCon");
	loseCon = GameplaySystems::GetGameObject("LoseCon");
	player = GameplaySystems::GetGameObject("Fang");
}

void WinLose::Update() {
	if (winCon == nullptr) return;
	if (loseCon == nullptr) return;
	if (player == nullptr) return;

	ComponentTransform* playerTransform = player->GetComponent<ComponentTransform>();
	ComponentTransform* winConTransform = winCon->GetComponent<ComponentTransform>();
	ComponentTransform* loseConTransform = loseCon->GetComponent<ComponentTransform>();
	if (!playerTransform || !winConTransform || !loseConTransform) return;

	float3 position = playerTransform->GetGlobalPosition();
	float3 winConPos = winConTransform->GetGlobalPosition();
	float3 loseConPos = loseConTransform->GetGlobalPosition();
	if (position.x <= winConPos.x + LoseOffsetX
		&& position.x >= winConPos.x - LoseOffsetX
		&& position.z <= winConPos.z + LoseOffsetZ
		&& position.z >= winConPos.z - LoseOffsetZ) {
		SceneManager::ChangeScene("Assets/Scenes/WinScene.scene");
	}
	else if (position.x <= loseConPos.x + LoseOffsetX
		&& position.x >= loseConPos.x - LoseOffsetX
		&& position.z <= loseConPos.z + LoseOffsetZ
		&& position.z >= loseConPos.z - LoseOffsetZ) {
		SceneManager::ChangeScene("Assets/Scenes/LoseScene.scene");
	}
}