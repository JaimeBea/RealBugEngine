#pragma once

#include "Panel.h"

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();

	void Update() override;

private:
	int window_width = 0;
	int window_height = 0;
};


