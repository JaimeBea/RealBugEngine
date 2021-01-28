#include "PanelInspector.h"

#include "Application.h"
#include "Resources/GameObject.h"
#include "Components/Component.h"
#include "Modules/ModuleEditor.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"

#include "GL/glew.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"

#include "Utils/Leaks.h"

PanelInspector::PanelInspector()
	: Panel("Inspector", true) {}

void PanelInspector::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockRightId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_CUBE " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		GameObject* selected = App->editor->selectedGameObject;
		if (selected != nullptr) {
			ImGui::TextUnformatted("Id:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%llu", selected->GetID());

			char name[100];
			sprintf_s(name, 100, "%s", selected->name.c_str());
			if (ImGui::InputText("Name", name, 100)) {
				selected->name = name;
			}
			bool active = selected->IsActive();
			if (ImGui::Checkbox("Active##game_object", &active)) {
				if (active) {
					selected->Enable();
				} else {
					selected->Disable();
				}
			}
			// TODO: Fix Me
			ImGui::SameLine();
			HelpMarker("To Fix it.");

			ImGui::Separator();

			for (Component* component : selected->components) {
				ImGui::PushID(component);
				component->OnEditorUpdate();
				ImGui::PopID();
			}
		}
	}
	ImGui::End();
}
