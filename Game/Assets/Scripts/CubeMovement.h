#pragma once

#include "Script.h"

class GameObject;

class CubeMovement : public Script
{
	GENERATE_BODY(CubeMovement);

public:

	void Start() override;
	void Update() override;

public:

	GameObject* gameObject = nullptr;

private:

	int speed = 10;
};
