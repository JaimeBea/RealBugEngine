#include "PanelScene.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "GameObject.h"
#include "Event.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBox.h"
#include "Resources/ResourcePrefab.h"
#include "Resources/ResourceScene.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "IconsFontAwesome5.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include "Geometry/OBB.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include <algorithm>
#include <Event.h>
#include "Utils/Leaks.h"

PanelScene::PanelScene()
	: Panel("Scene", true) {}

void PanelScene::Update() {
	if (!App->input->GetMouseButton(SDL_BUTTON_RIGHT)) {
		if (App->input->GetKey(SDL_SCANCODE_W)) currentGuizmoOperation = ImGuizmo::TRANSLATE; // W key
		if (App->input->GetKey(SDL_SCANCODE_E)) currentGuizmoOperation = ImGuizmo::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R)) currentGuizmoOperation = ImGuizmo::SCALE; // R key
	}

	ImGui::SetNextWindowDockID(App->editor->dockMainId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_BORDER_ALL " ") + name;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollWithMouse;
	if (ImGui::Begin(windowName.c_str(), &enabled, flags)) {
		// MenuBar Scene
		if (ImGui::BeginMenuBar()) {
			// Play / Pause / Step buttons
			if (App->time->HasGameStarted()) {
				if (ImGui::Button("Stop")) {
					App->BroadCastEvent(Event(Event::EventType::PRESSED_STOP));
				}
				ImGui::SameLine();
				if (App->time->IsGameRunning()) {
					if (ImGui::Button("Pause")) {
						App->BroadCastEvent(Event(Event::EventType::PRESSED_PAUSE));
					}
				} else {
					if (ImGui::Button("Resume")) {
						App->BroadCastEvent(Event(Event::EventType::PRESSED_RESUME));
					}
				}
			} else {
				if (ImGui::Button("Play")) {
					App->BroadCastEvent(Event(Event::EventType::PRESSED_PLAY));
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Step")) {
				App->BroadCastEvent(Event(Event::EventType::PRESSED_STEP));
			}

			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

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
			ImGui::PopItemWidth();

			ImGui::EndMenuBar();
		}

		// Update viewport size
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (App->renderer->viewportWidth != size.x || App->renderer->viewportHeight != size.y) {
			App->camera->ViewportResized((int) size.x, (int) size.y);
			App->renderer->ViewportResized((int) size.x, (int) size.y);
			framebufferSize = {
				size.x,
				size.y,
			};
		}

		ImVec2 framebufferPosition = ImGui::GetWindowPos();
		framebufferPosition.y += (ImGui::GetWindowHeight() - size.y);

		// Draw
		ImGui::Image((void*) App->renderer->renderTexture, size, ImVec2(0, 1), ImVec2(1, 0));

		// Drag and drop
		if (ImGui::BeginDragDropTarget()) {
			std::string payloadTypePrefab = std::string("_RESOURCE_") + GetResourceTypeName(ResourceType::PREFAB);
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadTypePrefab.c_str())) {
				UID prefabId = *(UID*) payload->Data;
				ResourcePrefab* prefab = (ResourcePrefab*) App->resources->GetResource(prefabId);
				if (prefab != nullptr) {
					prefab->BuildPrefab(App->scene->scene->root);
				}
			}

			// TODO: "Are you sure?" Popup to avoid losing the current scene
			std::string payloadTypeScene = std::string("_RESOURCE_") + GetResourceTypeName(ResourceType::SCENE);
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadTypeScene.c_str())) {
				UID sceneId = *(UID*) payload->Data;
				ResourceScene* scene = (ResourceScene*) App->resources->GetResource(sceneId);
				if (scene != nullptr) {
					scene->BuildScene();
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Capture input
		if (ImGui::IsWindowFocused()) {
			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) || App->input->GetKey(SDL_SCANCODE_LALT)) {
				ImGuizmo::Enable(false);
			} else {
				ImGuizmo::Enable(true);
			}

			ImGui::CaptureKeyboardFromApp(false);
			ImGui::CaptureMouseFromApp(true);
			ImGuiIO& io = ImGui::GetIO();
			mousePosOnScene.x = io.MousePos.x - framebufferPosition.x;
			mousePosOnScene.y = io.MousePos.y - framebufferPosition.y;

			if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && !ImGuizmo::IsOver()) {
				float2 mousePosNormalized;
				mousePosNormalized.x = -1 + 2 * std::max(-1.0f, std::min((io.MousePos.x - framebufferPosition.x) / (size.x), 1.0f));
				mousePosNormalized.y = 1 - 2 * std::max(-1.0f, std::min((io.MousePos.y - framebufferPosition.y) / (size.y), 1.0f));
				App->camera->CalculateFrustumNearestObject(mousePosNormalized);
			}
			ImGui::CaptureMouseFromApp(false);
		}

		float viewManipulateRight = framebufferPosition.x + framebufferSize.x;
		float viewManipulateTop = framebufferPosition.y;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(framebufferPosition.x, framebufferPosition.y, framebufferSize.x, framebufferSize.y);

		Frustum& engineFrustum = App->camera->GetEngineFrustum();
		float4x4 cameraView = float4x4(engineFrustum.ViewMatrix()).Transposed();
		float4x4 cameraProjection = engineFrustum.ProjectionMatrix().Transposed();

		GameObject* selectedGameObject = App->editor->selectedGameObject;
		if (selectedGameObject) {
			ComponentTransform* transform = selectedGameObject->GetComponent<ComponentTransform>();
			float4x4 globalMatrix = transform->GetGlobalMatrix().Transposed();

			ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), currentGuizmoOperation, currentGuizmoMode, globalMatrix.ptr(), NULL, useSnap ? snap : NULL);

			if (ImGuizmo::IsUsing()) {
				GameObject* parent = selectedGameObject->GetParent();
				float4x4 inverseParentMatrix = float4x4::identity;
				if (parent != nullptr) {
					ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();
					inverseParentMatrix = parentTransform->GetGlobalMatrix().Inverted();
				}
				float4x4 localMatrix = inverseParentMatrix * globalMatrix.Transposed();

				float3 translation;
				Quat rotation;
				float3 scale;
				localMatrix.Decompose(translation, rotation, scale);

				switch (currentGuizmoOperation) {
				case ImGuizmo::TRANSLATE:
					transform->SetPosition(translation);
					break;
				case ImGuizmo::ROTATE:
					transform->SetRotation(rotation);
					break;
				case ImGuizmo::SCALE:
					transform->SetScale(scale);
					break;
				}
			}
		}

		float viewManipulateSize = 100;
		ImGuizmo::ViewManipulate(cameraView.ptr(), 4, ImVec2(viewManipulateRight - viewManipulateSize, viewManipulateTop), ImVec2(viewManipulateSize, viewManipulateSize), 0x10101010);

		float4x4 newCameraView = cameraView.InverseTransposed();
		App->camera->engineCameraFrustum.SetFrame(newCameraView.Col(3).xyz(), -newCameraView.Col(2).xyz(), newCameraView.Col(1).xyz());

		ImGui::End();
		ImGui::PopStyleVar();
	}
}

float2 PanelScene::GetMousePosOnScene() {
	return mousePosOnScene;
}