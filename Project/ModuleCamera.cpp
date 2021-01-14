#include "ModuleCamera.h"

#include "Globals.h"
#include "GameObject.h"
#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "MeshImporter.h"
#include "MSTimer.h"
#include "Logging.h"

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

#include "Leaks.h"

static void WarpMouseOnEdges()
{
	const float2& mouse_position = App->input->GetMousePosition();

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(App->window->window), &display_mode);
	int screen_width = display_mode.w;
	int screen_height = display_mode.h;

	if (mouse_position.x < 20)
	{
		App->input->WarpMouse(screen_width - 22, (int) mouse_position.y);
	}
	if (mouse_position.y < 20)
	{
		App->input->WarpMouse((int) mouse_position.x, screen_height - 22);
	}
	if (mouse_position.x > screen_width - 20)
	{
		App->input->WarpMouse(22, (int) mouse_position.y);
	}
	if (mouse_position.y > screen_height - 20)
	{
		App->input->WarpMouse((int) mouse_position.x, 22);
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
			Focus(App->editor->panel_hierarchy.selected_object);
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
	const AABB& world_bounding_box = bounding_box->GetAABBWorldBoundingBox();
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

void ModuleCamera::ChangeFrustrum(Frustum& frustum_, bool change)
{
	if (change)
	{
		active_frustum = &frustum_;
	}
	else
	{
		active_frustum = &engine_camera_frustum;
	}
}

bool ModuleCamera::EngineFrustumActived()
{
	return (active_frustum == &engine_camera_frustum);
}

void ModuleCamera::CalculateFrustumNearestObject(float2 pos)
{
	MSTimer timer;
	timer.Start();

	if (active_frustum != &engine_camera_frustum) return;

	std::list<GameObject*> intersected_game_objects;
	LineSegment ray = engine_camera_frustum.UnProjectLineSegment(pos.x, pos.y);

	// Check with AABB
	for (GameObject& game_object : App->scene->game_objects)
	{
		ComponentBoundingBox* aabb = game_object.GetComponent<ComponentBoundingBox>();
		if (!aabb) continue;

		if (ray.Intersects(aabb->GetAABBWorldBoundingBox()))
		{
			intersected_game_objects.push_back(&game_object);
		}
	}

	GameObject* selected_game_object = nullptr;
	float min_distance = inf;
	for (GameObject* game_object : intersected_game_objects)
	{
		ComponentMesh* mesh = game_object->GetComponent<ComponentMesh>();
		if (mesh)
		{
			float4x4 model = game_object->GetComponent<ComponentTransform>()->GetGlobalMatrix();
			float distance;
			std::list<Triangle> triangles;
			MeshImporter::ExtractMeshTriangles(mesh->mesh, triangles, model);
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
	if (selected_game_object)
	{
		LOG("Selected: %s", selected_game_object->name.c_str());
		App->editor->panel_hierarchy.SetSelectedObject(selected_game_object);
	}

	unsigned time_ms = timer.Stop();
	LOG("Ray Tracing in %ums", time_ms);
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
