#pragma once

#include "Script.h"

class Camera : public Script
{
	GENERATE_BODY(Camera);

public:

	void Start() override;
	void Update() override;

};

