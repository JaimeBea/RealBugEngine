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

	GameObject* winCon = nullptr;
	GameObject* loseCon = nullptr;
	GameObject* player = nullptr;

public:
	float LoseOffsetX = 2.0f;
	float LoseOffsetZ = 2.0f;
};
