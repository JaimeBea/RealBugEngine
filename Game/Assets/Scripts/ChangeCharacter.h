#pragma once

#include "Script.h"

class GameObject;
class ComponentText;

#define COOLDOWN_TIME 5.f

class ChangeCharacter : public Script
{
	GENERATE_BODY(ChangeCharacter);

public:

	void Start() override;
	void Update() override;

public:

	GameObject* fang = nullptr;
	GameObject* robot = nullptr;

	/* Provisional code to see cooldown until we implement the proper UI */

	GameObject* canvas = nullptr;
	ComponentText* cooldownText = nullptr;

private:

	float timeLeft = 0.f;
	bool changeAvailable = true;
};

