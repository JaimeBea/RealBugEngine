#include "PanelHierarchy.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Resources/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleScene.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

#include "Utils/Leaks.h"

PanelHierarchy::PanelHierarchy()
	: Panel("Hierarchy", true) {}

void PanelHierarchy::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockLeftId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_SITEMAP " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		GameObject* root = App->scene->root;
		if (root != nullptr) {
			UpdateHierarchyNode(root);
		}
	}
	ImGui::End();
}

void PanelHierarchy::UpdateHierarchyNode(GameObject* gameObject) {
	const std::vector<GameObject*>& children = gameObject->GetChildren();

	char label[160];
	sprintf_s(label, "%s###%p", gameObject->name.c_str(), gameObject);

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags flags = baseFlags;

	if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
	bool isSelected = App->editor->selectedGameObject == gameObject;
	if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

	bool open = ImGui::TreeNodeEx(label, flags);

	ImGui::PushID(label);
	if (ImGui::BeginPopupContextItem("Options")) {
		if (gameObject != App->scene->root) {
			if (ImGui::Selectable("Delete")) {
				App->scene->DestroyGameObject(gameObject);
				if (isSelected) App->editor->selectedGameObject = nullptr;
			}

			ImGui::Selectable("Duplicate");
			ImGui::SameLine();
			HelpMarker("To implement");
			// TODO: Duplicate Objects

			ImGui::Separator();
		}

		if (ImGui::Selectable("Create Empty")) {
			GameObject* newGameObject = App->scene->CreateGameObject(gameObject);
			newGameObject->name = "Game Object";
			ComponentTransform* transform = newGameObject->CreateComponent<ComponentTransform>();
			transform->SetPosition(float3(0, 0, 0));
			transform->SetRotation(Quat::identity);
			transform->SetScale(float3(1, 1, 1));
			newGameObject->InitComponents();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();

	if (ImGui::IsItemClicked()) {
		App->editor->selectedGameObject = gameObject;
	}

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("_HIERARCHY", gameObject, sizeof(GameObject*));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_HIERARCHY")) {
			if (!gameObject->IsDescendantOf(App->editor->selectedGameObject)) {
				// multiplicar per inversa
				// obtenir parent global matrix
				// inverse
				// inverse * global local
				// set
				App->editor->selectedGameObject->SetParent(gameObject);
				ComponentTransform* transform = App->editor->selectedGameObject->GetComponent<ComponentTransform>();
				transform->InvalidateHierarchy();
				transform->CalculateGlobalMatrix();
			}
		} 
		ImGui::EndDragDropTarget();
	}

	if (open) {
		for (unsigned i = 0; i < children.size(); i++) {
			UpdateHierarchyNode(children[i]);
		}
		ImGui::TreePop();
	}
}
