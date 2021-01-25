#pragma once

#include "Module.h"

#include "Utils/Buffer.h"
#include "Panels/PanelScene.h"
#include "Panels/PanelConsole.h"
#include "Panels/PanelConfiguration.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelHierarchy.h"
#include "Panels/PanelAbout.h"

#include "imgui.h"
#include <vector>

class Panel;

enum class Modal {
	NONE,
	NEW_SCENE,
	LOAD_SCENE,
	SAVE_SCENE,
	QUIT
};

class ModuleEditor : public Module {
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

public:
	Modal modal_to_open = Modal::NONE;

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

	GameObject* selected_object = nullptr;

	ImVec4 title_color = ImVec4(0.35f, 0.69f, 0.87f, 1.0f);
	ImVec4 text_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	float drag_speed1f = 0.5f;
	float drag_speed2f = 0.05f;
	float drag_speed3f = 0.005f;
	float drag_speed5f = 0.00005f;

private:
	char file_name_buffer[32] = {'\0'};
};
