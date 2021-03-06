#include "ModuleCamera.h"

#include "Globals.h"
#include "Logging.h"
#include "Application.h"
#include "MeshImporter.h"
#include "GameObject.h"
#include "MSTimer.h"
#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleEditor.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Triangle.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include "SDL_video.h"
#include "Brofiler.h"
#include <vector>

#include "Leaks.h"

static void WarpMouseOnEdges()
{
	const float2& mouse_position = App->input->GetMousePosition();

	SDL_DisplayMode display_mode;
	int display_index = SDL_GetWindowDisplayIndex(App->window->window);
	SDL_GetCurrentDisplayMode(display_index, &display_mode);
	SDL_Rect display_bounds;
	SDL_GetDisplayBounds(display_index, &display_bounds);
	int screen_width = display_mode.w;
	int screen_height = display_mode.h;

	if (mouse_position.x < display_bounds.x + 20)
	{
		App->input->WarpMouse(display_bounds.x + display_bounds.w - 22, (int) mouse_position.y);
	}
	if (mouse_position.y < display_bounds.y + 20)
	{
		App->input->WarpMouse((int) mouse_position.x, display_bounds.y + display_bounds.h - 22);
	}
	if (mouse_position.x > display_bounds.x + display_bounds.w - 20)
	{
		App->input->WarpMouse(display_bounds.x + 22, (int) mouse_position.y);
	}
	if (mouse_position.y > display_bounds.y + display_bounds.h - 20)
	{
		App->input->WarpMouse((int) mouse_position.x, display_bounds.y + 22);
	}
}

bool ModuleCamera::Init()
{
	active_frustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	active_frustum->SetViewPlaneDistances(0.1f, 2000.0f);
	active_frustum->SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	active_frustum->SetFront(vec::unitZ);
	active_frustum->SetUp(vec::unitY);

	SetPosition(vec(2, 3, -5));
	LookAt(0, 0, 0);

	return true;
}

UpdateStatus ModuleCamera::Update()
{
	BROFILER_CATEGORY("ModuleCamera - Update", Profiler::Color::Blue)

	if (active_frustum != &engine_camera_frustum) return UpdateStatus::CONTINUE;

	float delta_time = App->time->GetRealTimeDeltaTime();

	const float2& mouse_motion = App->input->GetMouseMotion();

	// Increase zoom and movement speed with shift
	float final_movement_speed = movement_speed;
	float final_zoom_speed = zoom_speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) || App->input->GetKey(SDL_SCANCODE_RSHIFT))
	{
		final_movement_speed *= shift_multiplier;
		final_zoom_speed *= shift_multiplier;
	}

	// Zoom with mouse wheel
	float mouse_wheel_motion = App->input->GetMouseWheelMotion();
	if (mouse_wheel_motion < -FLT_EPSILON || mouse_wheel_motion > FLT_EPSILON)
	{
		Zoom(mouse_wheel_motion * 0.1f * focus_distance);
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT))
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT))
		{
			WarpMouseOnEdges();

			// Orbit with alt + left mouse button
			vec old_focus = active_frustum->Pos() + active_frustum->Front().Normalized() * focus_distance;
			Rotate(float3x3::RotateAxisAngle(active_frustum->WorldRight().Normalized(), -mouse_motion.y * rotation_speed * DEGTORAD));
			Rotate(float3x3::RotateY(-mouse_motion.x * rotation_speed * DEGTORAD));
			vec new_focus = active_frustum->Pos() + active_frustum->Front().Normalized() * focus_distance;
			Translate(old_focus - new_focus);
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
		{
			WarpMouseOnEdges();

			// Zoom with alt + right mouse button
			Zoom(mouse_motion.y * final_zoom_speed * focus_distance);
		}
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
	{
		WarpMouseOnEdges();

		// Rotate with mouse motion
		Rotate(float3x3::RotateAxisAngle(active_frustum->WorldRight().Normalized(), -mouse_motion.y * rotation_speed * DEGTORAD));
		Rotate(float3x3::RotateY(-mouse_motion.x * rotation_speed * DEGTORAD));

		// Move with WASD + QE
		if (App->input->GetKey(SDL_SCANCODE_Q))
		{
			Translate(active_frustum->Up().Normalized() * -final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_E))
		{
			Translate(active_frustum->Up().Normalized() * final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_W))
		{
			Translate(active_frustum->Front().Normalized() * final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_S))
		{
			Translate(active_frustum->Front().Normalized() * -final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_A))
		{
			Translate(active_frustum->WorldRight().Normalized() * -final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_D))
		{
			Translate(active_frustum->WorldRight().Normalized() * final_movement_speed * focus_distance * delta_time);
		}
	}
	else
	{
		// Focus camera around geometry with f key
		if (App->input->GetKey(SDL_SCANCODE_F))
		{
			Focus(App->editor->selected_object);
		}

		// Move with arrow keys
		if (App->input->GetKey(SDL_SCANCODE_UP))
		{
			Translate(active_frustum->Front().Normalized() * final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN))
		{
			Translate(active_frustum->Front().Normalized() * -final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT))
		{
			Translate(active_frustum->WorldRight().Normalized() * -final_movement_speed * focus_distance * delta_time);
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT))
		{
			Translate(active_frustum->WorldRight().Normalized() * final_movement_speed * focus_distance * delta_time);
		}
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::ViewportResized(int width, int height)
{
	SetAspectRatio(width / (float) height);
}

void ModuleCamera::SetFOV(float h_fov)
{
	active_frustum->SetHorizontalFovAndAspectRatio(h_fov, active_frustum->AspectRatio());
}

void ModuleCamera::SetAspectRatio(float aspect_ratio)
{
	active_frustum->SetVerticalFovAndAspectRatio(active_frustum->VerticalFov(), aspect_ratio);
}

void ModuleCamera::SetPlaneDistances(float near_plane, float far_plane)
{
	active_frustum->SetViewPlaneDistances(near_plane, far_plane);
}

void ModuleCamera::SetPosition(const vec& position)
{
	active_frustum->SetPos(position);
}

void ModuleCamera::SetPosition(float x, float y, float z)
{
	SetPosition(vec(x, y, z));
}

void ModuleCamera::SetOrientation(const float3x3& rotationMatrix)
{
	active_frustum->SetFront(rotationMatrix * float3::unitZ);
	active_frustum->SetUp(rotationMatrix * float3::unitY);
}

void ModuleCamera::Translate(const vec& translation)
{
	active_frustum->SetPos(active_frustum->Pos() + translation);
}

void ModuleCamera::Zoom(float amount)
{
	Translate(active_frustum->Front().Normalized() * amount);
	focus_distance -= amount;
	if (focus_distance < 0.0f)
	{
		focus_distance = 0.0f;
	}
}

void ModuleCamera::Rotate(const float3x3& rotationMatrix)
{
	vec oldFront = active_frustum->Front().Normalized();
	vec oldUp = active_frustum->Up().Normalized();
	active_frustum->SetFront(rotationMatrix * oldFront);
	active_frustum->SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::LookAt(float x, float y, float z)
{
	vec direction = vec(x, y, z) - active_frustum->Pos();
	focus_distance = direction.Length();
	direction.Normalize();
	vec up = vec::unitY;

	// Special case for when looking straight up
	if (direction.Cross(up).IsZero())
	{
		up = vec::unitZ;
	}

	Rotate(float3x3::LookAt(active_frustum->Front().Normalized(), direction, active_frustum->Up().Normalized(), up));
}

void ModuleCamera::Focus(const GameObject* game_object)
{
	if (game_object == nullptr) return;

	ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();
	if (!bounding_box) return;
	const AABB& world_bounding_box = bounding_box->GetWorldAABB();
	if (!world_bounding_box.IsFinite()) return;

	Sphere bounding_sphere = world_bounding_box.MinimalEnclosingSphere();
	float min_half_angle = Min(active_frustum->HorizontalFov(), active_frustum->VerticalFov()) * 0.5f;
	float relative_distance = bounding_sphere.r / Sin(min_half_angle);
	vec camera_direction = -active_frustum->Front().Normalized();
	vec camera_position = bounding_sphere.pos + (camera_direction * relative_distance);
	vec model_center = bounding_sphere.pos;
	SetPosition(camera_position);
	LookAt(model_center.x, model_center.y, model_center.z);
}

void ModuleCamera::ChangeActiveFrustum(Frustum& frustum, bool change)
{
	if (change)
	{
		active_frustum = &frustum;
	}
	else
	{
		active_frustum = &engine_camera_frustum;
	}
}

void ModuleCamera::CalculateFrustumNearestObject(float2 pos)
{
	MSTimer timer;
	timer.Start();

	if (active_frustum != &engine_camera_frustum) return;

	std::vector<GameObject*> intersecting_objects;
	LineSegment ray = engine_camera_frustum.UnProjectLineSegment(pos.x, pos.y);

	// Check with AABB
	for (GameObject& game_object : App->scene->game_objects)
	{
		game_object.flag = false;
		if (game_object.is_in_quadtree) continue;

		ComponentBoundingBox* bounding_box = game_object.GetComponent<ComponentBoundingBox>();
		if (bounding_box == nullptr) continue;

		const AABB& game_object_aabb = bounding_box->GetWorldAABB();
		if (ray.Intersects(game_object_aabb))
		{
			intersecting_objects.push_back(&game_object);
		}
	}
	if (App->scene->quadtree.IsOperative())
	{
		GetIntersectingAABBRecursive(App->scene->quadtree.root, App->scene->quadtree.bounds, ray, intersecting_objects);
	}

	GameObject* selected_game_object = nullptr;
	float min_distance = inf;
	float distance = 0;
	for (GameObject* game_object : intersecting_objects)
	{
		std::vector<ComponentMesh*> meshes = game_object->GetComponents<ComponentMesh>();
		for (ComponentMesh* mesh : meshes)
		{
			const float4x4& model = game_object->GetComponent<ComponentTransform>()->GetGlobalMatrix();
			std::vector<Triangle> triangles = MeshImporter::ExtractMeshTriangles(mesh->mesh, model);
			for (Triangle& triangle : triangles)
			{
				if (ray.Intersects(triangle, &distance, NULL))
				{
					if (distance < min_distance)
					{
						selected_game_object = game_object;
						min_distance = distance;
					}
				}
			}
		}
	}

	if (selected_game_object != nullptr)
	{
		App->editor->selected_object = selected_game_object;
	}

	LOG("Ray Tracing in %ums", timer.Stop());
}



void ModuleCamera::GetIntersectingAABBRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& node_aabb, const LineSegment& ray, std::vector<GameObject*>& intersecting_objects)
{
	AABB node_aabb_3d = AABB({node_aabb.minPoint.x, -1000000.0f, node_aabb.minPoint.y}, {node_aabb.maxPoint.x, 1000000.0f, node_aabb.maxPoint.y});
	if (ray.Intersects(node_aabb_3d))
	{
		if (node.IsBranch())
		{
			vec2d center = node_aabb.minPoint + (node_aabb.maxPoint - node_aabb.minPoint) * 0.5f;

			const Quadtree<GameObject>::Node& top_left = node.child_nodes->nodes[0];
			AABB2D top_left_aabb = {{node_aabb.minPoint.x, center.y}, {center.x, node_aabb.maxPoint.y}};
			GetIntersectingAABBRecursive(top_left, top_left_aabb, ray, intersecting_objects);

			const Quadtree<GameObject>::Node& top_right = node.child_nodes->nodes[1];
			AABB2D top_right_aabb = {{center.x, center.y}, {node_aabb.maxPoint.x, node_aabb.maxPoint.y}};
			GetIntersectingAABBRecursive(top_right, top_right_aabb, ray, intersecting_objects);

			const Quadtree<GameObject>::Node& bottom_left = node.child_nodes->nodes[2];
			AABB2D bottom_left_aabb = {{node_aabb.minPoint.x, node_aabb.minPoint.y}, {center.x, center.y}};
			GetIntersectingAABBRecursive(bottom_left, bottom_left_aabb, ray, intersecting_objects);

			const Quadtree<GameObject>::Node& bottom_right = node.child_nodes->nodes[3];
			AABB2D bottom_right_aabb = {{center.x, node_aabb.minPoint.y}, {node_aabb.maxPoint.x, center.y}};
			GetIntersectingAABBRecursive(bottom_right, bottom_right_aabb, ray, intersecting_objects);
		}
		else
		{
			const Quadtree<GameObject>::Element* element = node.first_element;
			while (element != nullptr)
			{
				GameObject* game_object = element->object;
				if (!game_object->flag)
				{
					ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();
					const AABB& game_object_aabb = bounding_box->GetWorldAABB();
					if (ray.Intersects(game_object_aabb))
					{
						intersecting_objects.push_back(game_object);
					}

					game_object->flag = true;
				}
				element = element->next;
			}
		}
	}
}

void ModuleCamera::ChangeCullingFrustum(Frustum& frustum, bool change)
{
	if (change)
	{
		culling_frustum = &frustum;
	}
	else
	{
		culling_frustum = &engine_camera_frustum;
	}
}

void ModuleCamera::CalculateFrustumPlanes()
{
	float3 pos = culling_frustum->Pos();
	float3 up = culling_frustum->Up().Normalized();
	float3 front = culling_frustum->Front();
	float3 right = culling_frustum->WorldRight().Normalized();
	float far_distance = culling_frustum->FarPlaneDistance();
	float near_distance = culling_frustum->NearPlaneDistance();
	float aspect_ratio = culling_frustum->AspectRatio();
	float vFOV = culling_frustum->VerticalFov();

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

	frustum_planes.planes[0] = culling_frustum->LeftPlane();
	frustum_planes.planes[1] = culling_frustum->RightPlane();
	frustum_planes.planes[2] = culling_frustum->TopPlane();
	frustum_planes.planes[3] = culling_frustum->BottomPlane();
	frustum_planes.planes[4] = culling_frustum->FarPlane();
	frustum_planes.planes[5] = culling_frustum->NearPlane();
}

vec ModuleCamera::GetFront() const
{
	return active_frustum->Front();
}

vec ModuleCamera::GetUp() const
{
	return active_frustum->Up();
}

vec ModuleCamera::GetWorldRight() const
{
	return active_frustum->WorldRight();
}

vec ModuleCamera::GetPosition() const
{
	return active_frustum->Pos();
}

float3 ModuleCamera::GetOrientation() const
{
	return active_frustum->ViewMatrix().RotatePart().ToEulerXYZ();
}

float ModuleCamera::GetFocusDistance() const
{
	return focus_distance;
}

float ModuleCamera::GetNearPlane() const
{
	return active_frustum->NearPlaneDistance();
}

float ModuleCamera::GetFarPlane() const
{
	return active_frustum->FarPlaneDistance();
}

float ModuleCamera::GetFOV() const
{
	return active_frustum->VerticalFov();
}

float ModuleCamera::GetAspectRatio() const
{
	return active_frustum->AspectRatio();
}

float4x4 ModuleCamera::GetProjectionMatrix() const
{
	return active_frustum->ProjectionMatrix();
}

float4x4 ModuleCamera::GetViewMatrix() const
{
	return active_frustum->ViewMatrix();
}

Frustum ModuleCamera::GetEngineFrustum() const
{
	return engine_camera_frustum;
}

Frustum* ModuleCamera::GetActiveFrustum() const
{
	return active_frustum;
}

Frustum* ModuleCamera::GetCullingFrustum() const
{
	return culling_frustum;
}

const FrustumPlanes& ModuleCamera::GetFrustumPlanes() const
{
	return frustum_planes;
}
