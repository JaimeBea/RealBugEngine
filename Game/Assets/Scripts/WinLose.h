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

public:
	float WinPointX = 13.0f;
	float LosePointX = -13.0f;
};
