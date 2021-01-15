#include "ModuleSceneRender.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentBoundingBox.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModulePrograms.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ComponentMesh.h"
#include "ComponentBoundingBox.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "SceneImporter.h"
#include "Logging.h"
#include "PerformanceTimer.h"

#include "debugdraw.h"
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

	// Draw the scene TODO: optimize
	for (GameObject& game_object : App->scene->game_objects)
	{
		game_object.flag = false;
	}
	App->camera->CalculateFrustumPlanes();
	PerformanceTimer timer;
	timer.Start();
	DrawSceneRecursive(App->scene->quadtree.root, App->scene->quadtree.bounds);
	LOG("Scene draw: %llu mis", timer.Stop());

	return UpdateStatus::CONTINUE;
}

void ModuleSceneRender::DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb)
{
	AABB aabb_3d = AABB({aabb.minPoint.x, -inf, aabb.minPoint.y}, {aabb.maxPoint.x, inf, aabb.maxPoint.y});
	if (true || CheckIfInsideFrustum(aabb_3d, aabb_3d))
	{
		if (node.IsBranch())
		{
			vec2d center = aabb.minPoint + (aabb.maxPoint - aabb.minPoint) * 0.5f;

			const Quadtree<GameObject>::Node& top_left = node.child_nodes->nodes[0];
			AABB2D top_left_aabb = {{aabb.minPoint.x, center.y}, {center.x, aabb.maxPoint.y}};
			DrawSceneRecursive(top_left, top_left_aabb);

			const Quadtree<GameObject>::Node& top_right = node.child_nodes->nodes[1];
			AABB2D top_right_aabb = {{center.x, center.y}, {aabb.maxPoint.x, aabb.maxPoint.y}};
			DrawSceneRecursive(top_right, top_right_aabb);

			const Quadtree<GameObject>::Node& bottom_left = node.child_nodes->nodes[2];
			AABB2D bottom_left_aabb = {{aabb.minPoint.x, aabb.minPoint.y}, {center.x, center.y}};
			DrawSceneRecursive(bottom_left, bottom_left_aabb);

			const Quadtree<GameObject>::Node& bottom_right = node.child_nodes->nodes[3];
			AABB2D bottom_right_aabb = {{center.x, aabb.minPoint.y}, {aabb.maxPoint.x, center.y}};
			DrawSceneRecursive(bottom_right, bottom_right_aabb);
		}
		else
		{
			if (true)
			{
				float3 points[8] = {
					{aabb.minPoint.x, 0, aabb.minPoint.y},
					{aabb.maxPoint.x, 0, aabb.minPoint.y},
					{aabb.maxPoint.x, 0, aabb.maxPoint.y},
					{aabb.minPoint.x, 0, aabb.maxPoint.y},
					{aabb.minPoint.x, 30, aabb.minPoint.y},
					{aabb.maxPoint.x, 30, aabb.minPoint.y},
					{aabb.maxPoint.x, 30, aabb.maxPoint.y},
					{aabb.minPoint.x, 30, aabb.maxPoint.y},
				};
				dd::box(points, dd::colors::White);
			}
			const Quadtree<GameObject>::Element* element = node.first_element;
			while (element != nullptr)
			{
				GameObject* game_object = element->object;
				if (!game_object->flag)
				{
					ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();
					const AABB& game_object_aabb = bounding_box->GetWorldAABB();
					const OBB& game_object_obb = bounding_box->GetWorldOBB();
					if (CheckIfInsideFrustum(game_object_aabb, game_object_obb))
					{
						DrawGameObject(game_object);
					}

					game_object->flag = true;
				}
				element = element->next;
			}
		}
	}
}

bool ModuleSceneRender::CheckIfInsideFrustum(const AABB& aabb, const OBB& obb)
{
	float3 points[8];
	obb.GetCornerPoints(points);

	const FrustumPlanes& frustum_planes = App->camera->GetFrustumPlanes();
	for (const Plane& plane : frustum_planes.planes)
	{
		// check box outside/inside of frustum
		int out = 0;
		for (int i = 0; i < 8; i++)
		{
			out += (plane.SignedDistance(points[i]) > 0 ? 1 : 0);
		}
		if (out == 8) return false;
	}

	// check frustum outside/inside box
	int out;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].x > aabb.MaxX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].x < aabb.MinX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].y > aabb.MaxY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].y < aabb.MinY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].z > aabb.MaxZ()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].z < aabb.MinZ()) ? 1 : 0);
	if (out == 8) return false;

	return true;
}

void ModuleSceneRender::DrawGameObject(GameObject* game_object)
{
	ComponentTransform* transform = game_object->GetComponent<ComponentTransform>();
	std::vector<ComponentMesh*> meshes = game_object->GetComponents<ComponentMesh>();
	std::vector<ComponentMaterial*> materials = game_object->GetComponents<ComponentMaterial>();
	ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();

	if (bounding_box && draw_all_bounding_boxes)
	{
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
		glDepthFunc(GL_LEQUAL);

		unsigned program = App->programs->skybox_program;
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, App->camera->GetViewMatrix().ptr());
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, App->camera->GetProjectionMatrix().ptr());
		glUniform1i(glGetUniformLocation(program, "cubemap"), 0);

		glBindVertexArray(App->scene->skybox_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, App->scene->skybox_cube_map->gl_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);
	}
}
