#include "PanelConsole.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Modules/ModuleEditor.h"

#include "imgui.h"
#include "IconsForkAwesome.h"

#include "Utils/Leaks.h"

static int ExecuteCommand(ImGuiInputTextCallbackData* data) {
	return 0;
}

PanelConsole::PanelConsole()
	: Panel("Console", true) {}

void PanelConsole::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockDownId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FK_TERMINAL " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		// Output
		const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::TextUnformattedWithColorMD(logString->c_str());
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();

		ImGui::Separator();

		// Command-line
		bool reclaimFocus = false;
		char inputBuf[256] = {0};
		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputTextFlags, &ExecuteCommand)) {
			logString->append("$FFB86CFF # ");
			logString->append(inputBuf);
			logString->append("\n");
			reclaimFocus = true;
		}
		ImGui::SetItemDefaultFocus();
		if (reclaimFocus) {
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
		}
	}
	ImGui::End();
}