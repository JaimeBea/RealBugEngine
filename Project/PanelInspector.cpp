#include "PanelInspector.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "Component.h"
#include "PanelHierarchy.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"

#include "GL/glew.h"
#include "imgui.h"

#include "Leaks.h"



PanelInspector::PanelInspector() : Panel("Inspector", true) {}

void PanelInspector::Update()
{
	ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		GameObject* selected = App->editor->panel_hierarchy.selected_object;
		if (selected != nullptr)
		{
			ImGui::TextUnformatted("Id:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%llu", selected->GetID());

			char name[100];
			sprintf_s(name, 100, "%s", selected->name.c_str());
			if (ImGui::InputText("Name", name, 100))
			{
				selected->name = name;
			}
			bool active = selected->IsActive();
			if (ImGui::Checkbox("Active##game_object", &active))
			{
				if (active)
				{
					selected->Enable();
				}
				else
				{
					selected->Disable();
				}
			}
			ImGui::Separator();

			for (Component* component : selected->components) {
				component->OnEditor();
			}
		}
	}
	ImGui::End();
}
