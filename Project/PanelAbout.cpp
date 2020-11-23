#include "PanelAbout.h"

#include "Application.h"

#include "imgui.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

PanelAbout::PanelAbout() : Panel("About", 400, 200, false) {}

void PanelAbout::Update()
{
    ImGui::Text("Engine Name:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->engine_name);
    ImGui::Text("Description:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->engine_description);
    ImGui::Text("Authors:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->engine_authors);
    ImGui::Text("Libraries:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->engine_libraries);
    ImGui::Text("License:");
    ImGui::SameLine();
    ImGui::TextColored(yellow, App->engine_license);
}
