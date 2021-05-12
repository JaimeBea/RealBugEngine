#pragma once

#include "Scripting/Script.h"

class ScreenResolutionConfirmer : public Script {
	GENERATE_BODY(ScreenResolutionConfirmer);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick()override;
};

