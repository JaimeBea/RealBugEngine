#include "ModuleCamera.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Model.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Geometry/Sphere.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include "SDL_video.h"

static void WarpMouseOnEdges()
{
    const float2& mouse_position = App->input->GetMousePosition();

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(App->window->window), &display_mode);
    int screen_width = display_mode.w;
    int screen_height = display_mode.h;

    if (mouse_position.x < 20)
    {
        App->input->WarpMouse(screen_width - 22, (int)mouse_position.y);
    }
    if (mouse_position.y < 20)
    {
        App->input->WarpMouse((int)mouse_position.x, screen_height - 22);
    }
    if (mouse_position.x > screen_width - 20)
    {
        App->input->WarpMouse(22, (int)mouse_position.y);
    }
    if (mouse_position.y > screen_height - 20)
    {
        App->input->WarpMouse((int)mouse_position.x, 22);
    }
}

bool ModuleCamera::Init()
{
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetViewPlaneDistances(0.1f, 2000.0f);
    frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
    frustum.SetFront(vec::unitZ);
    frustum.SetUp(vec::unitY);

    SetPosition(vec(2, 3, -5));
    LookAt(0, 0, 0);

    return true;
}

UpdateStatus ModuleCamera::Update()
{
    float delta_time = App->GetDeltaTime();

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
        Zoom(mouse_wheel_motion * 20 * delta_time);
    }

    if (App->input->GetKey(SDL_SCANCODE_LALT))
    {
        if (App->input->GetMouseButton(SDL_BUTTON_LEFT))
        {
            WarpMouseOnEdges();

            // Orbit with alt + left mouse button
            vec old_focus = frustum.Pos() + frustum.Front().Normalized() * focus_distance;
            Rotate(float3x3::RotateAxisAngle(frustum.WorldRight().Normalized(), -mouse_motion.y * rotation_speed * DEGTORAD * delta_time));
            Rotate(float3x3::RotateY(-mouse_motion.x * rotation_speed * DEGTORAD * delta_time));
            vec new_focus = frustum.Pos() + frustum.Front().Normalized() * focus_distance;
            Translate(old_focus - new_focus);
        }
        else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
        {
            WarpMouseOnEdges();

            // Zoom with alt + right mouse button
            Zoom(mouse_motion.y * final_zoom_speed * delta_time);
        }
    }
    else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
    {
        WarpMouseOnEdges();

        // Rotate with mouse motion
        Rotate(float3x3::RotateAxisAngle(frustum.WorldRight().Normalized(), -mouse_motion.y * rotation_speed * DEGTORAD * delta_time));
        Rotate(float3x3::RotateY(-mouse_motion.x * rotation_speed * DEGTORAD * delta_time));

        // Move with WASD + QE
        if (App->input->GetKey(SDL_SCANCODE_Q))
        {
            Translate(frustum.Up().Normalized() * -final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_E))
        {
            Translate(frustum.Up().Normalized() * final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_W))
        {
            Translate(frustum.Front().Normalized() * final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_S))
        {
            Translate(frustum.Front().Normalized() * -final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_A))
        {
            Translate(frustum.WorldRight().Normalized() * -final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_D))
        {
            Translate(frustum.WorldRight().Normalized() * final_movement_speed * delta_time);
        }
    }
    else
    {
        // Focus camera around geometry with f key
        if (App->input->GetKey(SDL_SCANCODE_F))
        {
            Focus(App->renderer->current_model);
        }

        // Move with arrow keys
        if (App->input->GetKey(SDL_SCANCODE_UP))
        {
            Translate(frustum.Front().Normalized() * final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_DOWN))
        {
            Translate(frustum.Front().Normalized() * -final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_LEFT))
        {
            Translate(frustum.WorldRight().Normalized() * -final_movement_speed * delta_time);
        }
        if (App->input->GetKey(SDL_SCANCODE_RIGHT))
        {
            Translate(frustum.WorldRight().Normalized() * final_movement_speed * delta_time);
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
    frustum.SetHorizontalFovAndAspectRatio(h_fov, frustum.AspectRatio());
}

void ModuleCamera::SetAspectRatio(float aspect_ratio)
{
    frustum.SetVerticalFovAndAspectRatio(frustum.VerticalFov(), aspect_ratio);
}

void ModuleCamera::SetPlaneDistances(float near_plane, float far_plane)
{
    frustum.SetViewPlaneDistances(near_plane, far_plane);
}

void ModuleCamera::SetPosition(const vec& position)
{
    frustum.SetPos(position);
}

void ModuleCamera::SetPosition(float x, float y, float z)
{
    SetPosition(vec(x, y, z));
}

void ModuleCamera::SetOrientation(const float3& orientaton)
{
    SetOrientation(orientaton.x, orientaton.y, orientaton.z);
}

void ModuleCamera::SetOrientation(float x, float y, float z)
{
    float3x3 rotationMatrix = float3x3::FromEulerXYZ(x, y, z);
    frustum.SetFront(rotationMatrix * float3::unitZ);
    frustum.SetUp(rotationMatrix * float3::unitY);
}

void ModuleCamera::Translate(const vec& translation)
{
    frustum.SetPos(frustum.Pos() + translation);
}

void ModuleCamera::Zoom(float amount)
{
    Translate(frustum.Front().Normalized() * amount);
    focus_distance -= amount;
    if (focus_distance < 0.0f)
    {
        focus_distance = 0.0f;
    }
}

void ModuleCamera::Rotate(const float3x3& rotationMatrix)
{
    vec oldFront = frustum.Front().Normalized();
    vec oldUp = frustum.Up().Normalized();
    frustum.SetFront(rotationMatrix * oldFront);
    frustum.SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::LookAt(float x, float y, float z)
{
    vec direction = vec(x, y, z) - frustum.Pos();
    focus_distance = direction.Length();
    direction.Normalize();
    Rotate(float3x3::LookAt(frustum.Front().Normalized(), direction, frustum.Up().Normalized(), vec::unitY));
}

void ModuleCamera::Focus(Model* model)
{
    float min_half_angle = Min(frustum.HorizontalFov(), frustum.VerticalFov()) * 0.5f;
    float relative_distance = model->bounding_sphere.r / Sin(min_half_angle);
    vec camera_direction = -frustum.Front().Normalized();
    vec camera_position = model->bounding_sphere.pos + (camera_direction * relative_distance);
    vec model_center = model->bounding_sphere.pos;
    SetPosition(camera_position);
    LookAt(model_center.x, model_center.y, model_center.z);
}

vec ModuleCamera::GetFront() const
{
    return frustum.Front();
}

vec ModuleCamera::GetUp() const
{
    return frustum.Up();
}

vec ModuleCamera::GetWorldRight() const
{
    return frustum.WorldRight();
}

vec ModuleCamera::GetPosition() const
{
    return frustum.Pos();
}

float3 ModuleCamera::GetOrientation() const
{
    return frustum.ViewMatrix().RotatePart().ToEulerXYZ();
}

float ModuleCamera::GetFocusDistance() const
{
    return focus_distance;
}

float ModuleCamera::GetNearPlane() const
{
    return frustum.NearPlaneDistance();
}

float ModuleCamera::GetFarPlane() const
{
    return frustum.FarPlaneDistance();
}

float ModuleCamera::GetFOV() const
{
    return frustum.VerticalFov();
}

float ModuleCamera::GetAspectRatio() const
{
    return frustum.AspectRatio();
}

float4x4 ModuleCamera::GetProjectionMatrix() const
{
    return frustum.ProjectionMatrix();
}

float4x4 ModuleCamera::GetViewMatrix() const
{
    return frustum.ViewMatrix();
}
