#pragma once

#include "Script.h"

class GameObject;

class EnemySpawnPoint : public Script {
	GENERATE_BODY(EnemySpawnPoint);

public:

	void Start() override;
	void Update() override;

private:
	GameObject* gameObject = nullptr;
	int speed = 0;
};
