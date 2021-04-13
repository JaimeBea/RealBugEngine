#pragma once

#include "Script.h"
class GameObject;

class StartButton : public Script
{
	GENERATE_BODY(StartButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
public:
	GameObject* Button = nullptr;
};
