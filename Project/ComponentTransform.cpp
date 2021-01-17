#include "ComponentTransform.h"

#include "Application.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentBoundingBox.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelScene.h"

#include "Math/float3x3.h"
#include "SDL.h"

#include "Leaks.h"

void ComponentTransform::Init()
{
	CalculateGlobalMatrix();
	for (Component* component : GetOwner().components)
	{
		component->OnTransformUpdate();
	}
}

void ComponentTransform::Update()
{
	CalculateGlobalMatrix();
}

void ComponentTransform::OnEditorUpdate()
{
	float3 pos = position;
	float3 scl = scale;
	float3 rot = local_euler_angles;

	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::TextColored(App->editor->title_color, "Transformation (X,Y,Z)");
		if (ImGui::DragFloat3("Position", pos.ptr(), App->editor->drag_speed2f, -inf, inf))
		{
			SetPosition(pos);
		}
		if (ImGui::DragFloat3("Scale", scl.ptr(), App->editor->drag_speed2f, 0, inf))
		{
			SetScale(scl);
		}

		if (ImGui::DragFloat3("Rotation", rot.ptr(), App->editor->drag_speed2f, -inf, inf))
		{
			SetRotation(rot);
		}
		ImGui::Separator();
	}
}

void ComponentTransform::Save(JsonValue j_component) const
{
	JsonValue j_position = j_component["Position"];
	j_position[0] = position.x;
	j_position[1] = position.y;
	j_position[2] = position.z;

	JsonValue j_rotation = j_component["Rotation"];
	j_rotation[0] = rotation.x;
	j_rotation[1] = rotation.y;
	j_rotation[2] = rotation.z;
	j_rotation[3] = rotation.w;

	JsonValue j_scale = j_component["Scale"];
	j_scale[0] = scale.x;
	j_scale[1] = scale.y;
	j_scale[2] = scale.z;

	JsonValue j_local_euler_angles = j_component["LocalEulerAngles"];
	j_local_euler_angles[0] = local_euler_angles.x;
	j_local_euler_angles[1] = local_euler_angles.y;
	j_local_euler_angles[2] = local_euler_angles.z;
}

void ComponentTransform::Load(JsonValue j_component)
{
	JsonValue j_position = j_component["Position"];
	position.Set(j_position[0], j_position[1], j_position[2]);

	JsonValue j_rotation = j_component["Rotation"];
	rotation.Set(j_rotation[0], j_rotation[1], j_rotation[2], j_rotation[3]);

	JsonValue j_scale = j_component["Scale"];
	scale.Set(j_scale[0], j_scale[1], j_scale[2]);

	JsonValue j_local_euler_angles = j_component["LocalEulerAngles"];
	local_euler_angles.Set(j_local_euler_angles[0], j_local_euler_angles[1], j_local_euler_angles[2]);

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
	ComponentBoundingBox* bounding_box = GetOwner().GetComponent<ComponentBoundingBox>();
	if (bounding_box) bounding_box->Invalidate();
}

void ComponentTransform::SetPosition(float3 position_)
{
	position = position_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components)
	{
		component->OnTransformUpdate();
	}
}

void ComponentTransform::SetRotation(Quat rotation_)
{
	rotation = rotation_;
	local_euler_angles = rotation_.ToEulerXYZ().Mul(RADTODEG);
	InvalidateHierarchy();
	for (Component* component : GetOwner().components)
	{
		component->OnTransformUpdate();
	}
}

void ComponentTransform::SetRotation(float3 rotation_)
{
	rotation = Quat::FromEulerXYZ(rotation_.x * DEGTORAD, rotation_.y * DEGTORAD, rotation_.z * DEGTORAD);
	local_euler_angles = rotation_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components)
	{
		component->OnTransformUpdate();
	}
}

void ComponentTransform::SetScale(float3 scale_)
{
	scale = scale_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components)
	{
		component->OnTransformUpdate();
	}
}

void ComponentTransform::CalculateGlobalMatrix(bool force)
{
	if (force || dirty)
	{
		local_matrix = float4x4::FromTRS(position, rotation, scale);

		GameObject* parent = GetOwner().GetParent();
		if (parent != nullptr)
		{
			ComponentTransform* parent_transform = parent->GetComponent<ComponentTransform>();

			parent_transform->CalculateGlobalMatrix();
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

const float4x4& ComponentTransform::GetLocalMatrix() const
{
	return local_matrix;
}

const float4x4& ComponentTransform::GetGlobalMatrix() const
{
	return global_matrix;
}

bool ComponentTransform::GetDirty() const
{
	return dirty;
}