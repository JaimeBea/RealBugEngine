#pragma once

#include "Module.h"

#include "Panel.h"
#include "PanelScene.h"
#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "PanelAbout.h"

#include <vector>

class ModuleEditor : public Module
{
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

private:
	std::vector<Panel*> panels;

	PanelScene panel_scene;
	PanelConsole panel_console;
	PanelConfiguration panel_configuration;
	PanelAbout panel_about;
};

