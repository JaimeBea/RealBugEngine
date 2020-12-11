#pragma once

#include "Module.h"

#include "Panel.h"
#include "PanelScene.h"
#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "PanelInspector.h"
#include "PanelHierarchy.h"
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
	unsigned dock_left_id = 0;
	unsigned dock_right_id = 0;
	unsigned dock_down_id = 0;

	std::vector<Panel*> panels;

	PanelScene panel_scene;
	PanelConsole panel_console;
	PanelConfiguration panel_configuration;
	PanelInspector panel_inspector;
	PanelHierarchy panel_hierarchy;
	PanelAbout panel_about;
};

