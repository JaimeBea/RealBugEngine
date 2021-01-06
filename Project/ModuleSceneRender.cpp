#include "ModuleSceneRender.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentBoundingBox.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"

#include "GL/glew.h"
#include "Geometry/Plane.h"
#include "Brofiler.h"
#include <list>

#include "Leaks.h"

UpdateStatus ModuleSceneRender::Update()
{
	BROFILER_CATEGORY("ModuleSceneRender - Update", Profiler::Color::DarkBlue)

	// Draw Skybox as a first element
	DrawSkyBox();

	// Load model or texture if one gets dropped
	const char* dropped_file_name = App->input->GetDroppedFileName();
	if (dropped_file_name != nullptr)
	{
		bool loaded_scene = App->scene->Load(dropped_file_name);
	}

	// Draw the scene

	GameObject* root = App->scene->root;
	if (root != nullptr)
	{
		drawable_game_objects.clear();
		frustum_normals.clear();

		for (GameObject* camera : App->scene->scene_cameras)
		{
			ComponentCamera* component_camera = camera->GetComponent<ComponentCamera>();
			if (component_camera->GetCullingStatus())
			{
				frustum_culling_active = true;
				frustum_normals.push_back(&GetFrustumNormals(component_camera->frustum));
			}
		}
		DrawScene(root);
	}

	for (GameObject* drawable_game_object : drawable_game_objects)
	{
		DrawGameObject(drawable_game_object);
	}

	frustum_culling_active = false;

	return UpdateStatus::CONTINUE;
}

void ModuleSceneRender::DrawScene(GameObject* game_object)
{
	if (frustum_culling_active)
	{
		for (FrustumNormals* frustum_normal : frustum_normals)
		{
			if (CheckIfInsideFrustum(game_object, frustum_normal))
			{
				drawable_game_objects.push_back(game_object);
				break;
			}
		}
	}
	else
	{
		drawable_game_objects.push_back(game_object);
	}

	for (GameObject* child : game_object->GetChildren())
	{
		DrawScene(child);
	}
}

FrustumNormals ModuleSceneRender::GetFrustumNormals(Frustum frustum)
{
	FrustumNormals frustum_normals;

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

	frustum_normals.points[0] = far_center + (up * h_far / 2) - (right * w_far / 2);
	frustum_normals.points[1] = far_center + (up * h_far / 2) + (right * w_far / 2);
	frustum_normals.points[2] = far_center - (up * h_far / 2) - (right * w_far / 2);
	frustum_normals.points[3] = far_center - (up * h_far / 2) + (right * w_far / 2);
	frustum_normals.points[4] = near_center + (up * h_near / 2) - (right * w_near / 2);
	frustum_normals.points[5] = near_center + (up * h_near / 2) + (right * w_near / 2);
	frustum_normals.points[6] = near_center - (up * h_near / 2) - (right * w_near / 2);
	frustum_normals.points[7] = near_center - (up * h_near / 2) + (right * w_near / 2);

	frustum_normals.planes[0] = frustum.LeftPlane();
	frustum_normals.planes[1] = frustum.RightPlane();
	frustum_normals.planes[2] = frustum.TopPlane();
	frustum_normals.planes[3] = frustum.BottomPlane();
	frustum_normals.planes[4] = frustum.FarPlane();
	frustum_normals.planes[5] = frustum.NearPlane();

	return frustum_normals;
}

bool ModuleSceneRender::CheckIfInsideFrustum(GameObject* game_object, FrustumNormals* frustum_normal)
{
	ComponentBoundingBox* boundig_box = game_object->GetComponent<ComponentBoundingBox>();
	if (!boundig_box) return false;

	float3 points[8];
	boundig_box->GetOBBWorldBoundingBox().GetCornerPoints(points);

	for (Plane plane : frustum_normal->planes)
	{
		// check box outside/inside of frustum
		int out = 0;
		for (int i = 0; i < 8; i++)
		{
			out += (plane.SignedDistance(points[i]) > 0 ? 1 : 0);
		}
		if (out == 8) return false;
	}

	AABB aabb = boundig_box->GetAABBWorldBoundingBox();

	// check frustum outside/inside box
	int out;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_normal->points[i].x > aabb.MaxX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_normal->points[i].x < aabb.MinX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_normal->points[i].y > aabb.MaxY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_normal->points[i].y < aabb.MinY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_normal->points[i].z > aabb.MaxZ()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_normal->points[i].z < aabb.MinZ()) ? 1 : 0);
	if (out == 8) return false;

	return true;
}

void ModuleSceneRender::DrawGameObject(GameObject* game_object)
{
	ComponentTransform* transform = game_object->GetComponent<ComponentTransform>();
	std::vector<ComponentMesh*> meshes = game_object->GetComponents<ComponentMesh>();
	std::vector<ComponentMaterial*> materials = game_object->GetComponents<ComponentMaterial>();
	ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();

	transform->CalculateGlobalMatrix();

	if (bounding_box && draw_all_bounding_boxes)
	{
		bounding_box->CalculateWorldBoundingBox();
		bounding_box->DrawBoundingBox();
	}

	for (ComponentMesh* mesh : meshes)
	{
		mesh->Draw(materials, transform->GetGlobalMatrix());
	}
}

void ModuleSceneRender::DrawSkyBox()
{
	if (skybox_active)
	{
		App->scene->DrawSkyBox();
	}
}
