#include "PanelScene.h"

#include "Globals.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBox.h"
#include "Application.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleProject.h"
#include "Utils/Logging.h"
#include "Event.h"
#include "Resources/ResourcePrefab.h"
#include "Resources/ResourceScene.h"
#include "Panels/PanelControlEditor.h"

#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "IconsFontAwesome5.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include "Geometry/OBB.h"
#include "SDL_mouse.h"
#include <algorithm>

#include "Utils/Leaks.h"

PanelScene::PanelScene()
	: Panel("Scene", true) {}

void PanelScene::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockMainId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_BORDER_ALL " ") + name;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollWithMouse;

	if (ImGui::Begin(windowName.c_str(), &enabled, flags)) {
		ImGuizmo::OPERATION currentGuizmoOperation = App->editor->panelControlEditor.GetImGuizmoOperation();
		ImGuizmo::MODE currentGuizmoMode = App->editor->panelControlEditor.GetImGuizmoMode();
		bool useSnap = App->editor->panelControlEditor.GetImGuizmoUseSnap();
		float snap[3];
		App->editor->panelControlEditor.GetImguizmoSnap(snap);

		if (ImGui::BeginMenuBar()) {
			ImGui::Text("Shadered");
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::Checkbox("2D", &view2D);

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
		if (ImGui::IsWindowFocused()) {
			float4x4 newCameraView = cameraView.InverseTransposed();
			App->camera->engineCameraFrustum.SetFrame(newCameraView.Col(3).xyz(), -newCameraView.Col(2).xyz(), newCameraView.Col(1).xyz());
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
}

float2 PanelScene::GetMousePosOnScene() const {
	return mousePosOnScene;
}

float2 PanelScene::GetSceneWindowSize() const {
	return framebufferSize;
}

const bool PanelScene::IsUsing2D() const {
	return view2D;
}
