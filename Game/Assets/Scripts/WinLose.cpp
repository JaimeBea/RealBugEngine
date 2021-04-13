#include "WinLose.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(WinLose);

void WinLose::Start() {
	gameObject = GameplaySystems::GetGameObject("Win");
	player = GameplaySystems::GetGameObject("Fang");
	loseText = GameplaySystems::GetGameObject("LoseText");
	winText = GameplaySystems::GetGameObject("WinText");
}

void WinLose::Update() {
	if (gameObject == nullptr) return;
	if (player == nullptr) return;
	if (loseText == nullptr) return;
	if (winText == nullptr) return;

	if (player->GetComponent<ComponentTransform>()->GetPosition().x >= WinPointX) {
		//player->GetComponent<ComponentTransform>()->SetPosition(float3(0.0f, 0.0f, 0.0f));
		winText->Enable();
		//SceneManager::SceneLoad("Assets/Scenes/LoseScene.scene");
	}
	else {
		winText->Disable();
	}
	if (player->GetComponent<ComponentTransform>()->GetPosition().x <= LosePointX) {
		//player->GetComponent<ComponentTransform>()->SetPosition(float3(0.0f, 0.0f, 0.0f));
		//	SceneManager::SceneLoad("Assets/Scenes/WinScene.scene");
		loseText->Enable();
	}
	else {
		loseText->Disable();
	}
}