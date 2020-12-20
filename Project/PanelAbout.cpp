#include "PanelAbout.h"

#include "Application.h"

#include "imgui.h"

#include "Leaks.h"

PanelAbout::PanelAbout()
	: Panel("About", false) {}

void PanelAbout::Update()
{
	ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextColored(title_color, "RealBug Engine");
		ImGui::Separator();
		ImGui::TextColored(title_color, u8"By Jaime Bea & Llorenç (Lowy) Solé.");
		ImGui::Text("Engine developed during the Master in Advanced Programming for AAA Video Games by UPC.");
		ImGui::Text("Engine licensed under the MIT License.");
		ImGui::Separator();

		ImVec2 child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 6);

		ImGui::Checkbox("Libraries", &show_library);
		if (show_library)
		{
			ImGui::BeginChildFrame(ImGui::GetID("cfg_infos"), child_size);
			ImGui::Text("MathGeoLib v1.5 ");
			ImGui::Text("Simple DirectMedia Layer (SDL) v2.0");
			ImGui::Text("ImGui Docking");
			ImGui::Text("OpenGL Extension Wrangler Library 2.1.0");
			ImGui::Text("DevIL 1.8.0");
			ImGui::Text("Assimp 1.4.1");

			ImGui::EndChildFrame();
		}
	}
	ImGui::End();
}
