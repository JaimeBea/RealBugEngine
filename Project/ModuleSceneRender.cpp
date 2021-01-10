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
		DrawGameObjectRecursive(root);
	}

	return UpdateStatus::CONTINUE;
}

void ModuleSceneRender::DrawGameObjectRecursive(GameObject* game_object)
{
	for (GameObject* camera : App->scene->scene_cameras)
	{
		ComponentCamera* component_camera = camera->GetComponent<ComponentCamera>();
		if (component_camera->GetCullingStatus())
		{
			frustum_culling_active = true;
			component_camera->UpdateFrustumPlanes();
			if (CheckIfInsideFrustum(game_object, &component_camera->frustum_planes))
			{
				DrawGameObject(game_object);
				break;
			}
		}
	}

	if (!frustum_culling_active)
	{
		DrawGameObject(game_object);
	}

	frustum_culling_active = false;

	for (GameObject* child : game_object->GetChildren())
	{
		DrawGameObjectRecursive(child);
	}
}

bool ModuleSceneRender::CheckIfInsideFrustum(GameObject* game_object, FrustumPlanes* frustum_planes)
{
	ComponentBoundingBox* boundig_box = game_object->GetComponent<ComponentBoundingBox>();
	if (!boundig_box) return false;

	float3 points[8];
	boundig_box->GetOBBWorldBoundingBox().GetCornerPoints(points);

	for (Plane& plane : frustum_planes->planes)
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
	for (int i = 0; i < 8; i++) out += ((frustum_planes->points[i].x > aabb.MaxX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes->points[i].x < aabb.MinX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes->points[i].y > aabb.MaxY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes->points[i].y < aabb.MinY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes->points[i].z > aabb.MaxZ()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes->points[i].z < aabb.MinZ()) ? 1 : 0);
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
