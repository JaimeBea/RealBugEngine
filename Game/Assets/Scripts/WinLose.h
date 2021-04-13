#pragma once

#include "Script.h"

class GameObject;

class WinLose : public Script
{
	GENERATE_BODY(WinLose);

public:

	void Start() override;
	void Update() override;
public:

	GameObject* gameObject = nullptr;
	GameObject* player = nullptr;
	GameObject* loseText = nullptr;
	GameObject* winText = nullptr;

public:
	float WinPointX = 230.0f;
	float LosePointX = -130.0f;
};
