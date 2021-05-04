#pragma once

#include "Scripting/Script.h"

class AIMovement : public Script
{
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;

private:
	bool CharacterInSight(GameObject* character);
	void Seek(GameObject* target);

public:

	GameObject owner;
	GameObject* fang = nullptr;
	GameObject* onimaru = nullptr;

private:

	int speed = 8;
	const float searchRadius = 50.f;

};

