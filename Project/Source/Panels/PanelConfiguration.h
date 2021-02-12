#pragma once

#include "Panel.h"

class PanelConfiguration : public Panel {
public:
	PanelConfiguration();

	void Update() override;

private:
	int windowWidth = 0;
	int windowHeight = 0;
};
