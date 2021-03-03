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
	COMPONENT_EXISTS,
	QUIT
};

class ModuleEditor : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

public:
	Modal modalToOpen = Modal::NONE; // Used in the MenuBar to popup a Modal Window of the specific type.

	unsigned dockMainId = 0;  // ??
	unsigned dockLeftId = 0;  // ??
	unsigned dockRightId = 0; // ??
	unsigned dockDownId = 0;  // ??

	// ---------- Panels ----------- //
	std::vector<Panel*> panels;
	PanelScene panelScene;
	PanelConsole panelConsole;
	PanelConfiguration panelConfiguration;
	PanelInspector panelInspector;
	PanelHierarchy panelHierarchy;
	PanelAbout panelAbout;

	GameObject* selectedGameObject = nullptr;			   // Pointer to the GameObject that will be shown in the inspector.
	ImVec4 titleColor = ImVec4(0.35f, 0.69f, 0.87f, 1.0f); // Color used for the titles in ImGui
	ImVec4 textColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);	   // Color used for text and information in ImGui

	// --- Float Sliders speeds ---- //
	float dragSpeed1f = 0.5f;
	float dragSpeed2f = 0.05f;
	float dragSpeed3f = 0.005f;
	float dragSpeed5f = 0.00005f;

private:
	char fileNameBuffer[32] = {'\0'}; // Buffer to show the name of the GameObject at the beginning of the Inspector
};
