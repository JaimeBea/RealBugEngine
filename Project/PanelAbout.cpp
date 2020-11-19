#include "PanelAbout.h"

#include "Application.h"
#include "ModuleConfig.h"

#include "imgui.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

PanelAbout::PanelAbout() : Panel("About", 200, 300, false) {}

void PanelAbout::Update()
{
    ImGui::Text("Engine Name:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->config->engine_name);
    ImGui::Text("Description:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->config->engine_description);
    ImGui::Text("Authors:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->config->engine_authors);
    ImGui::Text("Libraries:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->config->engine_libraries);
    ImGui::Text("License:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->config->engine_license);
}
