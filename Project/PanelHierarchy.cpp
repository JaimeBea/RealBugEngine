#include "PanelHierarchy.h"

#include "Logging.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "imgui.h"

#include "Leaks.h"

PanelHierarchy::PanelHierarchy()
	: Panel("Hierarchy", true) {}

void PanelHierarchy::Update()
{
	ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		GameObject* root = App->scene->root;
		if (root != nullptr)
		{
			UpdateHierarchyNode(root);
		}
	}
	ImGui::End();
}

void PanelHierarchy::UpdateHierarchyNode(GameObject* game_object)
{
	const std::vector<GameObject*>& children = game_object->GetChildren();

	char label[160];
	sprintf_s(label, "%s###%p", game_object->name.c_str(), game_object);

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags flags = base_flags;

	if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
	bool is_selected = App->editor->selected_object == game_object;
	if (is_selected) flags |= ImGuiTreeNodeFlags_Selected;

	bool open = ImGui::TreeNodeEx(label, flags);

	ImGui::PushID(label);
	if (ImGui::BeginPopupContextItem("Options"))
	{
		if (game_object != App->scene->root)
		{
			if (ImGui::Selectable("Delete"))
			{
				App->scene->DestroyGameObject(game_object);
				if (is_selected) App->editor->selected_object = nullptr;
			}

			ImGui::Selectable("Duplicate");
			ImGui::SameLine();
			HelpMarker("To implement");
			// TODO: Duplicate Objects

			ImGui::Separator();
		}

		if (ImGui::Selectable("Create Empty"))
		{
			GameObject* new_object = App->scene->CreateGameObject(game_object);
			new_object->name = "Game Object";
			ComponentTransform* transform = new_object->CreateComponent<ComponentTransform>();
			transform->SetPosition(float3(0, 0, 0));
			transform->SetRotation(Quat::identity);
			transform->SetScale(float3(1, 1, 1));
			transform->CalculateGlobalMatrix();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();

	if (ImGui::IsItemClicked())
	{
		App->editor->selected_object = game_object;
	}

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("_HIERARCHY", game_object, sizeof(GameObject*));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_HIERARCHY"))
		{
			if (!game_object->IsDescendantOf(App->editor->selected_object))
			{
				App->editor->selected_object->SetParent(game_object);
				ComponentTransform* transform = App->editor->selected_object->GetComponent<ComponentTransform>();
				transform->InvalidateHierarchy();
				transform->CalculateGlobalMatrix();
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (open)
	{
		for (unsigned i = 0; i < children.size(); i++)
		{
			UpdateHierarchyNode(children[i]);
		}
		ImGui::TreePop();
	}
}
