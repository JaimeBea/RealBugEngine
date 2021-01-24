#include "ComponentCamera.h"

#include "Globals.h"
#include "Application.h"
#include "Resources/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleCamera.h"

#include "imgui.h"
#include "debugdraw.h"
#include "Math/float3x3.h"
#include "Math/Quat.h"

#include "Utils/Leaks.h"

#define JSON_TAG_FRUSTRUM "Frustum"
#define JSON_TAG_POS "Pos"
#define JSON_TAG_UP "Up"
#define JSON_TAG_FRONT "Front"
#define JSON_TAG_NEAR_PLANE_DISTANCE "NearPlaneDistance"
#define JSON_TAG_FAR_PLANE_DISTANCE "FarPlaneDistance"
#define JSON_TAG_HORIZONTAL_FOV "HorizontalFov"
#define JSON_TAG_VERTICAL_FOV "VerticalFov"
#define JSON_TAG_CAMERA_SELECTED "CameraSelected"

void ComponentCamera::DrawGizmos() {
	if (active_camera) return;

	dd::frustum(frustum.ViewProjMatrix().Inverted(), dd::colors::White);
}

void ComponentCamera::OnTransformUpdate() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	frustum.SetPos(transform->GetPosition());

	float3x3 rotationMatrix = float3x3::FromQuat(transform->GetRotation());
	frustum.SetFront(rotationMatrix * float3::unitZ);
	frustum.SetUp(rotationMatrix * float3::unitY);
}

void ComponentCamera::OnEditorUpdate() {
	if (ImGui::CollapsingHeader("Camera")) {
		if (ImGui::Checkbox("Main Camera", &active_camera)) {
			App->camera->ChangeActiveFrustum(frustum, active_camera);
		}
		ImGui::Separator();

		vec front = frustum.Front();
		vec up = frustum.Up();
		ImGui::TextColored(App->editor->title_color, "Frustum");
		ImGui::InputFloat3("Front", front.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Up", up.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);

		float near_plane = frustum.NearPlaneDistance();
		float far_plane = frustum.FarPlaneDistance();
		if (ImGui::DragFloat("Near Plane", &near_plane, 0.1f, 0.0f, far_plane, "%.2f")) {
			frustum.SetViewPlaneDistances(near_plane, far_plane);
		}
		if (ImGui::DragFloat("Far Plane", &far_plane, 1.0f, near_plane, inf, "%.2f")) {
			frustum.SetViewPlaneDistances(near_plane, far_plane);
		}
		float fov = frustum.VerticalFov();
		if (ImGui::InputFloat("Field of View", &fov, 0.0F, 0.0F, "%.2f")) {
			frustum.SetHorizontalFovAndAspectRatio(fov, frustum.AspectRatio());
		}
		if (ImGui::Checkbox("Frustum Culling", &culling_camera)) {
			App->camera->ChangeCullingFrustum(frustum, culling_camera);
		}
	}
}

void ComponentCamera::Save(JsonValue j_component) const {
	JsonValue j_frustum = j_component[JSON_TAG_FRUSTRUM];
	JsonValue j_pos = j_frustum[JSON_TAG_POS];
	j_pos[0] = frustum.Pos().x;
	j_pos[1] = frustum.Pos().y;
	j_pos[2] = frustum.Pos().z;
	JsonValue j_up = j_frustum[JSON_TAG_UP];
	j_up[0] = frustum.Up().x;
	j_up[1] = frustum.Up().y;
	j_up[2] = frustum.Up().z;
	JsonValue j_front = j_frustum[JSON_TAG_FRONT];
	j_front[0] = frustum.Front().x;
	j_front[1] = frustum.Front().y;
	j_front[2] = frustum.Front().z;
	j_frustum[JSON_TAG_NEAR_PLANE_DISTANCE] = frustum.NearPlaneDistance();
	j_frustum[JSON_TAG_FAR_PLANE_DISTANCE] = frustum.FarPlaneDistance();
	j_frustum[JSON_TAG_HORIZONTAL_FOV] = frustum.HorizontalFov();
	j_frustum[JSON_TAG_VERTICAL_FOV] = frustum.VerticalFov();

	j_component[JSON_TAG_CAMERA_SELECTED] = active_camera;
}

void ComponentCamera::Load(JsonValue j_component) {
	JsonValue j_frustum = j_component[JSON_TAG_FRUSTRUM];
	JsonValue j_pos = j_frustum[JSON_TAG_POS];
	JsonValue j_up = j_frustum[JSON_TAG_UP];
	JsonValue j_front = j_frustum[JSON_TAG_FRONT];
	frustum.SetFrame(vec(j_pos[0], j_pos[1], j_pos[2]), vec(j_front[0], j_front[1], j_front[2]), vec(j_up[0], j_up[1], j_up[2]));
	frustum.SetViewPlaneDistances(j_frustum[JSON_TAG_NEAR_PLANE_DISTANCE], j_frustum[JSON_TAG_FAR_PLANE_DISTANCE]);
	frustum.SetPerspective(j_frustum[JSON_TAG_HORIZONTAL_FOV], j_frustum[JSON_TAG_VERTICAL_FOV]);

	active_camera = j_component[JSON_TAG_CAMERA_SELECTED];
}

Frustum ComponentCamera::BuildDefaultFrustum() const {
	Frustum new_frustum;
	new_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	new_frustum.SetViewPlaneDistances(0.1f, 200.0f);
	new_frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	new_frustum.SetFront(vec::unitZ);
	new_frustum.SetUp(vec::unitY);
	new_frustum.SetPos(vec::zero);
	return new_frustum;
}
