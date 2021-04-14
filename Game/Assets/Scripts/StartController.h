#pragma once

#include "Script.h"

class StartController : public Script
{
	GENERATE_BODY(StartController);

public:

	void Start() override;
	void Update() override;

};

