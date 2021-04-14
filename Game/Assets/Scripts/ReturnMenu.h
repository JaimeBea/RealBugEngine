#pragma once

#include "Script.h"

class GameObject;
class ReturnMenu : public Script
{
	GENERATE_BODY(ReturnMenu);

public:
	void Start() override;
	void Update() override;
	void OnButtonClick() override;

public:
	GameObject* button = nullptr;

	float padding = 20;
};

