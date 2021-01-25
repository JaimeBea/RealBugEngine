#pragma once

#include "Panel.h"

#include "imgui_file_explorer.h"

class PanelProject : public Panel {
public:
	PanelProject();

	void Update() override;

private:

	ImGui::FileExplorer fileExplorer;

};
