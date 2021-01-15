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
	if (active_camera) return;

	dd::frustum(frustum.ViewProjMatrix().Inverted(), dd::colors::White);
}

void ComponentCamera::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;

	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Checkbox("Main Camera", &active_camera))
		{
			App->camera->ChangeActiveFrustum(frustum, active_camera);
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
		if (ImGui::Checkbox("Frustum Culling", &culling_camera))
		{
			App->camera->ChangeCullingFrustum(frustum, culling_camera);
		}
	}
}

void ComponentCamera::Save(JsonValue& j_component) const
{
	JsonValue& j_frustum = j_component["Frustum"];
	JsonValue& j_pos = j_frustum["Pos"];
	j_pos[0] = frustum.Pos().x;
	j_pos[1] = frustum.Pos().y;
	j_pos[2] = frustum.Pos().z;
	JsonValue& j_up = j_frustum["Up"];
	j_up[0] = frustum.Up().x;
	j_up[1] = frustum.Up().y;
	j_up[2] = frustum.Up().z;
	JsonValue& j_front = j_frustum["Front"];
	j_front[0] = frustum.Front().x;
	j_front[1] = frustum.Front().y;
	j_front[2] = frustum.Front().z;
	j_frustum["NearPlaneDistance"] = frustum.NearPlaneDistance();
	j_frustum["FarPlaneDistance"] = frustum.FarPlaneDistance();
	j_frustum["HorizontalFov"] = frustum.HorizontalFov();
	j_frustum["VerticalFov"] = frustum.VerticalFov();

	j_component["CameraSelected"] = active_camera;
}

void ComponentCamera::Load(const JsonValue& j_component)
{
	const JsonValue& j_frustum = j_component["Frustum"];
	const JsonValue& j_pos = j_frustum["Pos"];
	const JsonValue& j_up = j_frustum["Up"];
	const JsonValue& j_front = j_frustum["Front"];
	frustum.SetFrame(vec(j_pos[0], j_pos[1], j_pos[2]), vec(j_front[0], j_front[1], j_front[2]), vec(j_up[0], j_up[1], j_up[2]));
	frustum.SetViewPlaneDistances(j_frustum["NearPlaneDistance"], j_frustum["FarPlaneDistance"]);
	frustum.SetPerspective(j_frustum["HorizontalFov"], j_frustum["VerticalFov"]);

	active_camera = j_component["CameraSelected"];
}
