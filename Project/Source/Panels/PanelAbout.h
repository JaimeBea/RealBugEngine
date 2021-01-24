#pragma once

#include "Panel.h"

class PanelAbout : public Panel {
public:
	PanelAbout();

	void Update() override;

private:
	bool show_library = false;
};
