#pragma once

#include "Script.h"

class GameObject;

class ChangeCharacter : public Script
{
	GENERATE_BODY(ChangeCharacter);

public:

	void Start() override;
	void Update() override;

public:

	GameObject* fang = nullptr;
	GameObject* robot = nullptr;
};

