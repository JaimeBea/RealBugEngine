#include "EnemySpawnPoint.h"

#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(EnemySpawnPoint);

void EnemySpawnPoint::Start() {
	gameObject = GameplaySystems::GetGameObject("Grunt");
}

void EnemySpawnPoint::Update() {
	if (!gameObject) return;
	if (!gameObject->IsActive()) gameObject->Enable();
}