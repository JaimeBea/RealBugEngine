#pragma once

#include "Script.h"

class ComponentAnimation;

class animationTest : public Script
{
	GENERATE_BODY(animationTest);

public:

	void Start() override;
	void Update() override;

private:
	ComponentAnimation* animation = nullptr;
};

