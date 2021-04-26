#pragma once

#include "Script.h"

class Rotate : public Script
{
	GENERATE_BODY(Rotate);

public:

	void Start() override;
	void Update() override;

};

