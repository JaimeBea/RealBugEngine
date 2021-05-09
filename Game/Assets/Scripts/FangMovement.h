#pragma once

#include "Scripting/Script.h"
#include "Character.h"

class GameObject;

class FangMovement : public Script
{
	GENERATE_BODY(FangMovement);

public:

	void Start() override;
	void Update() override;

public:
	UID cameraUID;
	UID fangUID;
	UID hitGOUID;
	int speed = 10;
	int distanceRayCast = 20;
	float cameraXPosition = -163;

private:
	GameObject* camera = nullptr;
	GameObject* fang = nullptr;
	GameObject* hitGo = nullptr;
	Character* character;
};
