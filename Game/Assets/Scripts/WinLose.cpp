#include "WinLose.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

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

	if (player->GetComponent<ComponentTransform>()->GetGlobalPosition().x <= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x + LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().x >= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x - LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z <= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z + LoseOffsetZ
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z >= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z - LoseOffsetZ) {
		SceneManager::SceneLoad("Assets/Scenes/WinScene.scene");
	}
	if (player->GetComponent<ComponentTransform>()->GetGlobalPosition().x <= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x + LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().x >= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x - LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z <= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z + LoseOffsetZ
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z >= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z - LoseOffsetZ) {
		SceneManager::SceneLoad("Assets/Scenes/LoseScene.scene");
	}
}