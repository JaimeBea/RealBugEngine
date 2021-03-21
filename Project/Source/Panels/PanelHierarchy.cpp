#include "PanelHierarchy.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Resources/ResourcePrefab.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleResources.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

#include "Utils/Leaks.h"

static ImVec4 grey = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
static ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

PanelHierarchy::PanelHierarchy()
	: Panel("Hierarchy", true) {}

void PanelHierarchy::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockLeftId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_SITEMAP " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		GameObject* root = App->scene->scene->root;
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

	//White for active gameobjects, gray for disabled objects, if a parent is not active, children are inherently not active
	ImGui::PushStyleColor(0, gameObject->IsActiveInHierarchy() ? white : grey);

	bool open = ImGui::TreeNodeEx(label, flags);

	ImGui::PopStyleColor();

	ImGui::PushID(label);
	if (ImGui::BeginPopupContextItem("Options")) {
		App->editor->selectedGameObject = gameObject;
		Scene* scene = App->scene->scene;
		if (gameObject != scene->root) {
			if (ImGui::Selectable("Delete")) {
				if (isSelected) App->editor->selectedGameObject = nullptr;
				App->scene->DestroyGameObjectDeferred(gameObject);
			}

			if (ImGui::Selectable("Duplicate")) {
				scene->DuplicateGameObject(gameObject, gameObject->GetParent());
			}

			ImGui::Separator();
		}

		if (ImGui::Selectable("Create Empty")) {
			GameObject* newGameObject = scene->CreateGameObject(gameObject, GenerateUID(), "Game Object");
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
		UID id = gameObject->GetID();
		ImGui::SetDragDropPayload("_HIERARCHY", &id, sizeof(UID));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		// Hierarchy movements
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_HIERARCHY")) {
			UID payloadGameObjectId = *(UID*) payload->Data;
			GameObject* payloadGameObject = App->scene->scene->GetGameObject(payloadGameObjectId);
			if (!gameObject->IsDescendantOf(payloadGameObject)) {
				ComponentTransform* transform = payloadGameObject->GetComponent<ComponentTransform>();
				ComponentTransform* parentTransform = gameObject->GetComponent<ComponentTransform>();
				// Recompute local matrix to maintain global position
				// 1. Get current matrix
				float4x4 childGlobalMatrix = transform->GetGlobalMatrix();
				float4x4 parentGlobalMatrix = parentTransform->GetGlobalMatrix();
				float3 parentScale = float3(parentGlobalMatrix.Col3(0).Length(), parentGlobalMatrix.Col3(1).Length(), parentGlobalMatrix.Col3(2).Length());
				// 2. Invert the new parent global matrix with the fastest possible method
				if (parentScale.Equals(float3::one)) { // No scaling
					parentGlobalMatrix.InverseOrthonormal();
				} else if (parentScale.xxx().Equals(parentScale)) { // Uniform scaling
					parentGlobalMatrix.InverseOrthogonalUniformScale();
				} else { // Non-uniform scaling
					parentGlobalMatrix.InverseColOrthogonal();
				}
				// 3. New local matrix
				transform->SetTRS(parentGlobalMatrix * childGlobalMatrix);

				payloadGameObject->SetParent(gameObject);
				transform->InvalidateHierarchy();
				transform->CalculateGlobalMatrix();
			}
		}

		// Prefabs
		std::string prafabPayloadType = std::string("_RESOURCE_") + GetResourceTypeName(ResourceType::PREFAB);
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(prafabPayloadType.c_str())) {
			UID prefabId = *(UID*) payload->Data;
			ResourcePrefab* prefab = (ResourcePrefab*) App->resources->GetResourceByID(prefabId);
			if (prefab != nullptr) {
				prefab->BuildPrefab(gameObject);
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
