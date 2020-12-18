#include "PanelHierarchy.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Logging.h"

#include "imgui.h"


#include "Leaks.h"


PanelHierarchy::PanelHierarchy() : Panel("Hierarchy", true) {}

void PanelHierarchy::Update()
{
	ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		GameObject* root = App->scene->root;
		if (root != nullptr)
		{
			if (ImGui::TreeNode("%d", root->name.c_str())) 
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::Selectable("Create Empty"))
					{
						GameObject* new_object = App->scene->CreateGameObject(root);
						new_object->name = "Game Object";
						ComponentTransform* transform = new_object->CreateComponent<ComponentTransform>();
						transform->SetPosition(float3(0, 0, 0));
						transform->SetRotation(Quat::identity);
						transform->SetScale(float3(1, 1, 1));
						transform->CalculateGlobalMatrix();
					}
					ImGui::EndPopup();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_HIERARCHY"))
					{
						selected_object->SetParent(root);
						ComponentTransform* transform = selected_object->GetComponent<ComponentTransform>();
						transform->InvalidateHierarchy();
						transform->CalculateGlobalMatrix();
					}
					ImGui::EndDragDropTarget();
				}
				UpdateHierarchyNode(root);
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void PanelHierarchy::UpdateHierarchyNode(GameObject* game_object)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	std::vector<GameObject*> children = game_object->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		char label[160];
		sprintf_s(label, "%s###%p", children[i]->name.c_str(), children[i]);
		ImGuiTreeNodeFlags flags = base_flags;
		if (children[i]->GetChildren().empty()) flags |= ImGuiTreeNodeFlags_Leaf;
		bool is_selected = (selected_id == children[i]->GetID());
		if (is_selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool open = ImGui::TreeNodeEx(label, flags);

		if (ImGui::BeginPopupContextItem("Options"))
		{
			if (ImGui::Selectable("Delete"))
			{
				App->scene->DestroyGameObject(selected_object);
				selected_object = nullptr;
			}
			ImGui::Selectable("Duplicate"); ImGui::SameLine(); HelpMarker("To implement");
			// NTH: Tool to duplicate Objects

			ImGui::Separator();
			if (ImGui::Selectable("Create Empty"))
			{
				GameObject* new_object = App->scene->CreateGameObject(selected_object);
				new_object->name = "Game Object";
				ComponentTransform* transform = new_object->CreateComponent<ComponentTransform>();
				transform->SetPosition(float3(0, 0, 0));
				transform->SetRotation(Quat::identity);
				transform->SetScale(float3(1, 1, 1));
				transform->CalculateGlobalMatrix();
			}
			ImGui::Separator();
			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked())
		{
			selected_object = children[i];
			selected_id = children[i]->GetID();
		}

		if (ImGui::BeginDragDropSource()) 
		{
			ImGui::SetDragDropPayload("_HIERARCHY", children[i], sizeof(GameObject*));
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_HIERARCHY"))
			{
				if (!children[i]->IsParent(selected_object)) {
					selected_object->SetParent(children[i]);
					ComponentTransform* transform = selected_object->GetComponent<ComponentTransform>();
					transform->InvalidateHierarchy();
					transform->CalculateGlobalMatrix();
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (open)
		{
			UpdateHierarchyNode(children[i]);
			ImGui::TreePop();
		}
	}
}

