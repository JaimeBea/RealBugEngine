#include "ModuleCamera.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"

#include "Math/float3x3.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"

bool ModuleCamera::Init()
{
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetViewPlaneDistances(0.1f, 200.0f);
    frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);

    frustum.SetPos(vec(0, 1, -2));
    frustum.SetFront(vec::unitZ);
    frustum.SetUp(vec::unitY);

    return true;
}

UpdateStatus ModuleCamera::PreUpdate()
{
    float delta_time = App->GetDeltaTime();

    float final_movement_speed = movement_speed;
    if (App->input->GetKey(SDL_SCANCODE_LSHIFT) || App->input->GetKey(SDL_SCANCODE_RSHIFT))
    {
        final_movement_speed *= 2;
    }

    float mouse_wheel_motion = App->input->GetMouseWheelMotion();
    if (mouse_wheel_motion < -FLT_EPSILON || mouse_wheel_motion > FLT_EPSILON)
    {
        Translate(frustum.Front().Normalized() * mouse_wheel_motion * 10 * zoom_speed * delta_time);
    }

    // TODO: Make mouse speed depend on screen size
    float2 mouse_motion = App->input->GetMouseMotion();
    KeyState left_mouse_button = App->input->GetMouseButton(SDL_BUTTON_LEFT);
    KeyState right_mouse_button = App->input->GetMouseButton(SDL_BUTTON_RIGHT);
    KeyState alt_key = App->input->GetKey(SDL_SCANCODE_LALT);
    if (left_mouse_button)
    {
        if (left_mouse_button == KS_DOWN)
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }
        else if (left_mouse_button == KS_UP)
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }

        Translate((frustum.Up().Normalized() * mouse_motion.y / 150.0f) + (frustum.WorldRight().Normalized() * -mouse_motion.x / 150.0f));
    }
    else if (alt_key && right_mouse_button)
    {
        if (right_mouse_button == KS_DOWN || alt_key == KS_DOWN)
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }
        else if (right_mouse_button == KS_UP || alt_key == KS_UP)
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }

        Translate(frustum.Front().Normalized() * mouse_motion.y / 150.0f);
    }

    if (App->input->GetKey(SDL_SCANCODE_Q))
    {
        Translate(vec::unitY * -final_movement_speed * delta_time);
    }
    if (App->input->GetKey(SDL_SCANCODE_E))
    {
        Translate(vec::unitY * final_movement_speed * delta_time);
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

    if (App->input->GetKey(SDL_SCANCODE_UP))
    {
        Rotate(float3x3::RotateAxisAngle(frustum.WorldRight().Normalized(), rotation_speed * DEGTORAD * delta_time));
    }
    if (App->input->GetKey(SDL_SCANCODE_DOWN))
    {
        Rotate(float3x3::RotateAxisAngle(frustum.WorldRight().Normalized(), -rotation_speed * DEGTORAD * delta_time));
    }
    if (App->input->GetKey(SDL_SCANCODE_LEFT))
    {
        Rotate(float3x3::RotateY(rotation_speed * DEGTORAD * delta_time));
    }
    if (App->input->GetKey(SDL_SCANCODE_RIGHT))
    {
        Rotate(float3x3::RotateY(-rotation_speed * DEGTORAD * delta_time));
    }

    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleCamera::Update()
{
    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleCamera::PostUpdate()
{
    return UpdateStatus::CONTINUE;
}

bool ModuleCamera::CleanUp()
{
    return true;
}

void ModuleCamera::WindowResized(int width, int height)
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

void ModuleCamera::SetPosition(vec position)
{
    frustum.SetPos(position);
}

void ModuleCamera::SetPosition(float x, float y, float z)
{
    SetPosition(vec(x, y, z));
}

void ModuleCamera::SetOrientation(float3 orientaton)
{
    SetOrientation(orientaton.x, orientaton.y, orientaton.z);
}

void ModuleCamera::SetOrientation(float x, float y, float z)
{
    float3x3 rotationMatrix = float3x3::FromEulerXYZ(x, y, z);
    frustum.SetFront(rotationMatrix * float3::unitZ);
    frustum.SetUp(rotationMatrix * float3::unitY);
}

void ModuleCamera::Translate(vec translation)
{
    frustum.SetPos(frustum.Pos() + translation);
}

void ModuleCamera::Rotate(float3x3 rotationMatrix)
{
    vec oldFront = frustum.Front().Normalized();
    vec oldUp = frustum.Up().Normalized();
    frustum.SetFront(rotationMatrix * oldFront);
    frustum.SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::LookAt(float x, float y, float z)
{
    vec direction = vec(x, y, z) - frustum.Pos();
    direction.Normalize();
    Rotate(float3x3::LookAt(frustum.Front().Normalized(), direction, frustum.Up().Normalized(), vec::unitY));
}

vec ModuleCamera::GetFront() const
{
    return frustum.Front();
}

vec ModuleCamera::GetUp() const
{
    return frustum.Up();
}

vec ModuleCamera::GetPosition() const
{
    return frustum.Pos();
}

float3 ModuleCamera::GetOrientation() const
{
    return frustum.ViewMatrix().RotatePart().ToEulerXYZ();
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
