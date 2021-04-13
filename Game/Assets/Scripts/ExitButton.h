#pragma once

#include "Script.h"
class GameObject;

class LoseButton : public Script
{
	GENERATE_BODY(LoseButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
public:
	GameObject* Button = nullptr;
};
