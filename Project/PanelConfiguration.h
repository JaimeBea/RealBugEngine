#pragma once

#include "Panel.h"

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();

	void Update() override;

private:
	int window_width = 0;
	int window_height = 0;
};

