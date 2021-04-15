#pragma once

#include "Script.h"

class CreditsButton : public Script
{
	GENERATE_BODY(CreditsButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
};
