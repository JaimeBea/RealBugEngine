#include "WinLose.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(WinLose);

void WinLose::Start() {
	gameObject = GameplaySystems::GetGameObject("Win");
	player = GameplaySystems::GetGameObject("Hearse");
}

void WinLose::Update() {
	if (player->GetComponent<ComponentTransform>()->GetPosition().x >= WinPointX) {
		player->GetComponent<ComponentTransform>()->SetPosition(float3(0.0f, 0.0f, 0.0f));
		SceneManager::SceneLoad("Assets/Scenes/LoseScene.scene");
	}
	if (player->GetComponent<ComponentTransform>()->GetPosition().x <= LosePointX) {
		player->GetComponent<ComponentTransform>()->SetPosition(float3(0.0f, 0.0f, 0.0f));
		SceneManager::SceneLoad("Assets/Scenes/WinScene.scene");
	}
}