#pragma once

#include "Scripting/Script.h"

class AIMovement : public Script
{
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;

};

