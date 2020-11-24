#pragma once

#include "Module.h"

#include "Panel.h"
#include "PanelScene.h"
#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "PanelProperties.h"
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

public:
	unsigned dock_space_id = 0;
	unsigned dock_main_id = 0;
	unsigned dock_right_id = 0;
	unsigned dock_down_id = 0;

private:
	std::vector<Panel*> panels;

	PanelScene panel_scene;
	PanelConsole panel_console;
	PanelConfiguration panel_configuration;
	PanelProperties panel_properties;
	PanelAbout panel_about;
};

