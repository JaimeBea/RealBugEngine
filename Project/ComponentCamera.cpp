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
	Invalidate();
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

		ImGui::Checkbox("Frustum Culling", &apply_frustum_culling);
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

	j_component["CameraSelected"] = camera_selected;
	j_component["ApplyFrustumCulling"] = apply_frustum_culling;
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

	camera_selected = j_component["CameraSelected"];
	apply_frustum_culling = j_component["ApplyFrustumCulling"];

	dirty = true;
}

void ComponentCamera::UpdateFrustumPlanes(bool force)
{
	if (dirty || force)
	{
		float3 pos = frustum.Pos();
		float3 up = frustum.Up().Normalized();
		float3 front = frustum.Front();
		float3 right = frustum.WorldRight().Normalized();
		float far_distance = frustum.FarPlaneDistance();
		float near_distance = frustum.NearPlaneDistance();
		float aspect_ratio = frustum.AspectRatio();
		float vFOV = frustum.VerticalFov();

		float h_far = 2 * tan(vFOV / 2) * far_distance;
		float w_far = h_far * aspect_ratio;
		float h_near = 2 * tan(vFOV / 2) * near_distance;
		float w_near = h_near * aspect_ratio;
		float3 far_center = pos + front * far_distance;
		float3 near_center = pos + front * near_distance;

		frustum_planes.points[0] = far_center + (up * h_far / 2) - (right * w_far / 2);
		frustum_planes.points[1] = far_center + (up * h_far / 2) + (right * w_far / 2);
		frustum_planes.points[2] = far_center - (up * h_far / 2) - (right * w_far / 2);
		frustum_planes.points[3] = far_center - (up * h_far / 2) + (right * w_far / 2);
		frustum_planes.points[4] = near_center + (up * h_near / 2) - (right * w_near / 2);
		frustum_planes.points[5] = near_center + (up * h_near / 2) + (right * w_near / 2);
		frustum_planes.points[6] = near_center - (up * h_near / 2) - (right * w_near / 2);
		frustum_planes.points[7] = near_center - (up * h_near / 2) + (right * w_near / 2);

		frustum_planes.planes[0] = frustum.LeftPlane();
		frustum_planes.planes[1] = frustum.RightPlane();
		frustum_planes.planes[2] = frustum.TopPlane();
		frustum_planes.planes[3] = frustum.BottomPlane();
		frustum_planes.planes[4] = frustum.FarPlane();
		frustum_planes.planes[5] = frustum.NearPlane();
	}
	dirty = false;
}

void ComponentCamera::Invalidate()
{
	dirty = true;
}

bool ComponentCamera::GetCullingStatus() const
{
	return apply_frustum_culling;
}
