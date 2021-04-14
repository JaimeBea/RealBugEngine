#pragma once

#include "Script.h"
class GameObject;

class HowToPlayButton : public Script
{
	GENERATE_BODY(HowToPlayButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
public:
	GameObject* Button = nullptr;
};
