#include "PanelInspector.h"

#include "Application.h"
#include "Resources/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentType.h"
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
			// Add New Components
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
			if (ImGui::Button("Add New Component", ImVec2(ImGui::GetContentRegionAvail().x, 25))) { ImGui::OpenPopup("AddComponentPopup"); }
			if (ImGui::BeginPopup("AddComponentPopup")) {
				// TODO: Missing functionality for meshRenderer with Bounding Box and Material
				ComponentType type = ComponentType::UNKNOWN;
				if (ImGui::MenuItem("Transform")) type = ComponentType::TRANSFORM;
				if (ImGui::MenuItem("Mesh Renderer")) type = ComponentType::MESH;
				if (ImGui::MenuItem("Camera")) type = ComponentType::CAMERA;
				if (ImGui::MenuItem("Light")) type = ComponentType::LIGHT;

				if (type != ComponentType::UNKNOWN) CreateComponentByType(selected, type);
				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}
