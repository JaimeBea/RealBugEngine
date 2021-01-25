#include "PanelProject.h"

#include "Application.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleScene.h"

#include "IconsForkAwesome.h"

PanelProject::PanelProject()
	: Panel("Project", true) {
	fileExplorer = ImGui::FileExplorer();
}

void PanelProject::Update() {
	ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FK_FOLDER " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		ImGui::Columns(2);

		fileExplorer.DrawFileTree("Library");

		ImGui::NextColumn();

		fileExplorer.ShowContent();

		ImGui::End();
	}
}