#include "ComponentCamera.h"

#include "Globals.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "PanelHierarchy.h"

#include "imgui.h"
#include "debugdraw.h"
#include "Math/float3x3.h"
#include "Math/Quat.h"

ComponentCamera::ComponentCamera(GameObject& owner)
	: Component(static_type, owner) {}

void ComponentCamera::Init()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	frustum.SetFront(vec::unitZ);
	frustum.SetUp(vec::unitY);

	GameObject& parent = this->GetOwner();
	ComponentTransform* transform = parent.GetComponent<ComponentTransform>();
	frustum.SetPos(transform->GetPosition());
}

void ComponentCamera::DrawGizmos()
{
	if (camera_selected) return;

	dd::frustum((frustum.ProjectionMatrix() * frustum.ViewMatrix()).Inverted(), dd::colors::White);
}

void ComponentCamera::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;

	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Checkbox("Main Camera", &camera_selected))
		{
			App->camera->ChangeFrustrum(frustum, camera_selected);
		}
		ImGui::Separator();

		vec front = frustum.Front();
		vec up = frustum.Up();
		ImGui::TextColored(title_color, "Frustum");
		ImGui::InputFloat3("Front", front.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Up", up.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);

		float near_plane = frustum.NearPlaneDistance();
		float far_plane = frustum.FarPlaneDistance();
		if (ImGui::DragFloat("Near Plane", &near_plane, 0.1f, 0.0f, far_plane, "%.2f"))
		{
			frustum.SetViewPlaneDistances(near_plane, far_plane);
		}
		if (ImGui::DragFloat("Far Plane", &far_plane, 1.0f, near_plane, inf, "%.2f"))
		{
			frustum.SetViewPlaneDistances(near_plane, far_plane);
		}
		float fov = frustum.VerticalFov();
		if (ImGui::InputFloat("Field of View", &fov, 0.0F, 0.0F, "%.2f"))
		{
			frustum.SetHorizontalFovAndAspectRatio(fov, frustum.AspectRatio());
		}

		if (ImGui::Checkbox("Frustum Culling", &apply_frustum_culling))
		{
		}
	}
}

bool ComponentCamera::GetCullingStatus() const
{
	return apply_frustum_culling;
}
