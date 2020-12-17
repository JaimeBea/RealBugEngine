#include "ComponentTransform.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui.h"

ComponentTransform::ComponentTransform(GameObject& owner) : Component(static_type, owner) {}

void ComponentTransform::OnEditor(){

	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	ComponentTransform* transform = selected->GetComponent<ComponentTransform>();
	float3 pos3 = transform->GetPosition();
	float pos[] = { pos3[0], pos3[1], pos3[2] };
	float3 scale3 = transform->GetScale();
	float scale[] = { scale3[0], scale3[1], scale3[2] };
	float3 rotation3 = transform->GetRotation().ToEulerXYZ() * RADTODEG;
	float rotation[] = { rotation3[0], rotation3[1], rotation3[2] };

	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::TextColored(title_color, "Transformation (X,Y,Z)");
		if (ImGui::DragFloat3("Position", pos, drag_speed2f, -inf, inf))
		{
			transform->SetPosition(float3(pos[0], pos[1], pos[2]));
		}
		if (ImGui::DragFloat3("Scale", scale, drag_speed2f, 0, inf))
		{
			transform->SetScale(float3(scale[0], scale[1], scale[2]));
		}
		if (ImGui::DragFloat3("Rotation", rotation, drag_speed2f, -inf, inf))
		{
			transform->SetRotation(Quat::FromEulerXYZ(rotation[0] * DEGTORAD, rotation[1] * DEGTORAD, rotation[2] * DEGTORAD));
		}
		ImGui::Separator();
	}
}

void ComponentTransform::InvalidateHierarchy()
{
	Invalidate();

	for (GameObject* child : GetOwner().GetChildren())
	{
		ComponentTransform* child_transform = child->GetComponent<ComponentTransform>();
		if (child_transform != nullptr)
		{
			child_transform->Invalidate();
		}
	}
}

void ComponentTransform::Invalidate()
{
	dirty = true;
}

void ComponentTransform::SetPosition(float3 position_)
{
	position = position_;
	InvalidateHierarchy();
}

void ComponentTransform::SetRotation(Quat rotation_)
{
	rotation = rotation_;
	InvalidateHierarchy();
}

void ComponentTransform::SetScale(float3 scale_)
{
	scale = scale_;
	InvalidateHierarchy();
}

void ComponentTransform::CalculateGlobalMatrix(bool force)
{
	if (force || dirty  || true)
	{
		local_matrix = float4x4::FromTRS(position, rotation, scale);

		GameObject* parent = GetOwner().GetParent();
		if (parent != nullptr)
		{
			ComponentTransform* parent_transform = parent->GetComponent<ComponentTransform>();

			global_matrix = parent_transform->global_matrix * local_matrix;
		}
		else
		{
			global_matrix = local_matrix;
		}

		dirty = false;
	}
}

float3 ComponentTransform::GetPosition() const
{
	return position;
}

Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

float3 ComponentTransform::GetScale() const
{
	return scale;
}

const float4x4& ComponentTransform::GetGlobalMatrix() const
{
	return global_matrix;
}
