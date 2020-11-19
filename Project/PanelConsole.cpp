#include "PanelConsole.h"

#include "Logging.h"

#include "imgui.h"

PanelConsole::PanelConsole() : Panel("Console", 600, 200) {}

void PanelConsole::Update()
{
	ImGui::TextUnformatted(log_string.c_str());
}
