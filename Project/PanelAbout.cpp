#include "PanelAbout.h"

#include "Application.h"

#include "imgui.h"

#include "Leaks.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

PanelAbout::PanelAbout() : Panel("About", false) {}

void PanelAbout::Update()
{
    ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(name, &enabled))
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
    ImGui::End();
}
