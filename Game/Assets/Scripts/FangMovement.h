#pragma once

#include "Script.h"

class GameObject;

class FangMovement : public Script
{
	GENERATE_BODY(FangMovement);

public:

	void Start() override;
	void Update() override;
public:

	GameObject* gameObject = nullptr;

private:

	int speed = 100;
};
