#include "StartButton.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(StartButton) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};


GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
}

void StartButton::Update() {
}

void StartButton::OnButtonClick() {
	SceneManager::ChangeScene("Assets/Scenes/Level1-34.scene");
	player = GameplaySystems::GetGameObject("Fang");

	if (player) {
		player->GetComponent<ComponentTransform>()->SetPosition(float3(37.f, 0.f, 41.0f));
	}
}