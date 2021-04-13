#pragma once

#include "Script.h"

class GameObject;

class PlayerMovement : public Script
{
	GENERATE_BODY(PlayerMovement);

public:

	void Start() override;
	void Update() override;

public:

	GameObject* gameObject = nullptr;

private:

	int speed = 10;
};
