#include "PanelControlEditor.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleEvents.h"
#include "Modules/ModuleTime.h"

#include "imgui_internal.h"
#include "IconsFontAwesome5.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include "Geometry/OBB.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include <algorithm>

#include "Utils/Leaks.h"

PanelControlEditor::PanelControlEditor()
	: Panel("Editor Control", true) {}

void PanelControlEditor::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockUpId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_TOOLS " ") + name;
	ImGuiWindowFlags windowsFlags = ImGuiWindowFlags_NoTitleBar;
	if (ImGui::Begin(windowName.c_str(), &enabled, windowsFlags)) {
		// Control Bar
		if (!App->input->GetMouseButton(SDL_BUTTON_RIGHT)) {
			if (App->input->GetKey(SDL_SCANCODE_W)) currentGuizmoOperation = ImGuizmo::TRANSLATE; // W key
			if (App->input->GetKey(SDL_SCANCODE_E)) currentGuizmoOperation = ImGuizmo::ROTATE;
			if (App->input->GetKey(SDL_SCANCODE_R)) currentGuizmoOperation = ImGuizmo::SCALE; // R key
		}

		if (ImGui::RadioButton("Translate", currentGuizmoOperation == ImGuizmo::TRANSLATE)) currentGuizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", currentGuizmoOperation == ImGuizmo::ROTATE)) currentGuizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", currentGuizmoOperation == ImGuizmo::SCALE)) currentGuizmoOperation = ImGuizmo::SCALE;

		if (currentGuizmoOperation != ImGuizmo::SCALE) {
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();
			if (ImGui::RadioButton("Local", currentGuizmoMode == ImGuizmo::LOCAL)) currentGuizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", currentGuizmoMode == ImGuizmo::WORLD)) currentGuizmoMode = ImGuizmo::WORLD;
		} else {
			currentGuizmoMode = ImGuizmo::LOCAL;
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		ImGui::TextColored(App->editor->titleColor, "Snap");
		ImGui::SameLine();
		ImGui::Checkbox("##snap", &useSnap);
		ImGui::SameLine();

		ImGui::PushItemWidth(150);
		switch (currentGuizmoOperation) {
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Snap Angle", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Snap Scale", &snap[0]);
			break;
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		// Play / Pause / Step buttons
		if (App->time->HasGameStarted()) {
			if (ImGui::Button("Stop")) {
				App->events->AddEvent(Event(EventType::PRESSED_STOP));
			}
			ImGui::SameLine();
			if (App->time->IsGameRunning()) {
				if (ImGui::Button("Pause")) {
					App->events->AddEvent(Event(EventType::PRESSED_PAUSE));
				}
			} else {
				if (ImGui::Button("Resume")) {
					App->events->AddEvent(Event(EventType::PRESSED_RESUME));
				}
			}
		} else {
			if (ImGui::Button("Play")) {
				for (auto it : App->scene->scene->scriptComponents) {
					it.OnStart();
				}
				App->events->AddEvent(Event(EventType::PRESSED_PLAY));
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Step")) {
			App->events->AddEvent(Event(EventType::PRESSED_STEP));
		}

		ImGui::PopItemWidth();
		ImGui::End();
	}
}

ImGuizmo::OPERATION PanelControlEditor::GetImGuizmoOperation() const {
	return currentGuizmoOperation;
}

ImGuizmo::MODE PanelControlEditor::GetImGuizmoMode() const {
	return currentGuizmoMode;
}

bool PanelControlEditor::GetImGuizmoUseSnap() const {
	return useSnap;
}

void PanelControlEditor::GetImguizmoSnap(float* newSnap) const {
	newSnap[0] = snap[0];
	newSnap[1] = snap[1];
	newSnap[2] = snap[2];
}