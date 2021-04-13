#pragma once

#include "Script.h"

class GameObject;

class FrangMovement : public Script
{
	GENERATE_BODY(FrangMovement);

public:

	void Start() override;
	void Update() override;
public:

	GameObject* gameObject = nullptr;

private:

	int speed = 100;
};
