#include "ComponentTransform.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentBoundingBox.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelScene.h"

#include "Math/float3x3.h"

void ComponentTransform::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;

	ComponentTransform* transform = selected->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();
	float3 scale = transform->GetScale();
	float3 rotation = transform->GetRotation().ToEulerXYZ() * RADTODEG;

	// if is a camera
	ComponentCamera* camera = selected->GetComponent<ComponentCamera>();

	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::IsKeyPressed(90)) // W key
			current_guizmo_operation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69)) // E key
			current_guizmo_operation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // R key
			current_guizmo_operation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", current_guizmo_operation == ImGuizmo::TRANSLATE))
			current_guizmo_operation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", current_guizmo_operation == ImGuizmo::ROTATE))
			current_guizmo_operation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", current_guizmo_operation == ImGuizmo::SCALE))
			current_guizmo_operation = ImGuizmo::SCALE;
		ImGui::Separator();

		ImGui::TextColored(title_color, "Transformation (X,Y,Z)");
		if (ImGui::DragFloat3("Position", pos.ptr(), drag_speed2f, -inf, inf))
		{
			transform->SetPosition(pos);
			if (camera != nullptr)
			{
				camera->Invalidate();
				camera->frustum.SetPos(pos);
			}
		}
		if (ImGui::DragFloat3("Scale", scale.ptr(), drag_speed2f, 0, inf))
		{
			transform->SetScale(scale);
		}

		// TODO: Fix Quaternion Angles
		if (ImGui::DragFloat3("Rotation", rotation.ptr(), drag_speed2f, -inf, inf))
		{
			transform->SetRotation(Quat::FromEulerXYZ(rotation[0] * DEGTORAD, rotation[1] * DEGTORAD, rotation[2] * DEGTORAD));
			if (camera != nullptr)
			{
				camera->Invalidate();
				float3x3 rotationMatrix = float3x3::FromQuat(GetRotation());
				camera->frustum.SetFront(rotationMatrix * float3::unitZ);
				camera->frustum.SetUp(rotationMatrix * float3::unitY);
			}
		}

		if (current_guizmo_operation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", current_guizmo_mode == ImGuizmo::LOCAL))
				current_guizmo_mode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", current_guizmo_mode == ImGuizmo::WORLD))
				current_guizmo_mode = ImGuizmo::WORLD;
		}
		ImGui::Checkbox("##snap", &useSnap);
		ImGui::SameLine();

		switch (current_guizmo_operation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}

		Frustum& default_frustum = App->camera->engine_camera_frustum;
		transform->UpdateTransform();
		float4x4 matrix = transform->GetGlobalMatrix();

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
	ComponentBoundingBox* bounding_box = GetOwner().GetComponent<ComponentBoundingBox>();
	if (bounding_box) bounding_box->Invalidate();
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
	if (force || dirty)
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

void ComponentTransform::UpdateTransform()
{
	CalculateGlobalMatrix();

	GameObject& owner = GetOwner();
	ComponentBoundingBox* bounding_box = owner.GetComponent<ComponentBoundingBox>();
	if (bounding_box) bounding_box->CalculateWorldBoundingBox();

	for (GameObject* child : owner.GetChildren())
	{
		ComponentTransform* transform = child->GetComponent<ComponentTransform>();
		transform->UpdateTransform();
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

ImGuizmo::OPERATION ComponentTransform::GetGizmoOperation() const
{
	return current_guizmo_operation;
}

ImGuizmo::MODE ComponentTransform::GetGizmoMode() const
{
	return current_guizmo_mode;
}
