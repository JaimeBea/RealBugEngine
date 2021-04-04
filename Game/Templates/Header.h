#pragma once

#include "Script.h"

class CameraScr : public Script
{
	GENERATE_BODY(CameraScr);

public:

	void Start() override;
	void Update() override;

};

