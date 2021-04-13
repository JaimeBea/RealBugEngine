#pragma once

#include "Script.h"
class GameObject;

class buttonTest : public Script
{
	GENERATE_BODY(buttonTest);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
public:
	GameObject* Button = nullptr;
};
