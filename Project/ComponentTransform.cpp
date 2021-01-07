#include "ComponentTransform.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui.h"

void ComponentTransform::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	ComponentTransform* transform = selected->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();
	float3 scale = transform->GetScale();
	float3 rotation = transform->GetRotation().ToEulerXYZ() * RADTODEG;

	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::TextColored(title_color, "Transformation (X,Y,Z)");
		if (ImGui::DragFloat3("Position", pos.ptr(), drag_speed2f, -inf, inf))
		{
			transform->SetPosition(pos);
		}
		if (ImGui::DragFloat3("Scale", scale.ptr(), drag_speed2f, 0, inf))
		{
			transform->SetScale(scale);
		}

		// TODO: Fix Quaternion Angles
		if (ImGui::DragFloat3("Rotation", rotation.ptr(), drag_speed2f, -inf, inf))
		{
			transform->SetRotation(Quat::FromEulerXYZ(rotation[0] * DEGTORAD, rotation[1] * DEGTORAD, rotation[2] * DEGTORAD));
		}
		ImGui::Separator();
	}
}

void ComponentTransform::Save(JsonValue& j_component) const
{
	JsonValue& j_position = j_component["Position"];
	j_position[0] = position.x;
	j_position[1] = position.y;
	j_position[2] = position.z;

	JsonValue& j_rotation = j_component["Rotation"];
	j_rotation[0] = rotation.x;
	j_rotation[1] = rotation.y;
	j_rotation[2] = rotation.z;
	j_rotation[3] = rotation.w;

	JsonValue& j_scale = j_component["Scale"];
	j_scale[0] = scale.x;
	j_scale[1] = scale.y;
	j_scale[2] = scale.z;
}

void ComponentTransform::Load(const JsonValue& j_component)
{
	const JsonValue& j_position = j_component["Position"];
	position.Set(j_position[0], j_position[1], j_position[2]);

	const JsonValue& j_rotation = j_component["Rotation"];
	rotation.Set(j_rotation[0], j_rotation[1], j_rotation[2], j_rotation[3]);

	const JsonValue& j_scale = j_component["Scale"];
	scale.Set(j_scale[0], j_scale[1], j_scale[2]);

	dirty = true;
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
	if (force || dirty || true)
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
