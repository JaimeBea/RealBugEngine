#include "WinLose.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(WinLose);

void WinLose::Start() {
	winCon = GameplaySystems::GetGameObject("WinCon");
	loseCon = GameplaySystems::GetGameObject("LoseCon");
	player = GameplaySystems::GetGameObject("Fang");
	loseText = GameplaySystems::GetGameObject("LoseText");
	winText = GameplaySystems::GetGameObject("WinText");
}

void WinLose::Update() {
	if (winCon == nullptr) return;
	if (loseCon == nullptr) return;
	if (player == nullptr) return;
	if (loseText == nullptr) return;
	if (winText == nullptr) return;

	if (player->GetComponent<ComponentTransform>()->GetGlobalPosition().x <= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x + LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().x >= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x - LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z <= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z + LoseOffsetZ
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z >= winCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z - LoseOffsetZ) {
		player->GetComponent<ComponentTransform>()->SetPosition(float3(-187.f, 3.f, -36.f));	// Reset Player position to Start point
		winText->Enable();
		//SceneManager::SceneLoad("Assets/Scenes/LoseScene.scene");
	}
	else {
		winText->Disable();
	}
	if (player->GetComponent<ComponentTransform>()->GetGlobalPosition().x <= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x + LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().x >= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().x - LoseOffsetX
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z <= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z + LoseOffsetZ
		&& player->GetComponent<ComponentTransform>()->GetGlobalPosition().z >= loseCon->GetComponent<ComponentTransform>()->GetGlobalPosition().z - LoseOffsetZ) {
		player->GetComponent<ComponentTransform>()->SetPosition(float3(-187.f, 3.f, -36.f));	// Reset Player position to Start point
		//	SceneManager::SceneLoad("Assets/Scenes/WinScene.scene");
		loseText->Enable();
	}
	else {
		loseText->Disable();
	}
}