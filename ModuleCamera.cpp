#include "ModuleCamera.h"
#include "GL/glew.h"
#include "Math/float3x3.h"

ModuleCamera::ModuleCamera()
{
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetViewPlaneDistances(0.1f, 200.0f);
    frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);

    frustum.SetPos(float3(0, 1, -2));
    frustum.SetFront(float3::unitZ);
    frustum.SetUp(float3::unitY);
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
    return true;
}

update_status ModuleCamera::PreUpdate()
{
    float4x4 projectionGL = GetProjectionMatrix(); //<-- Important to transpose!

    //Send the frustum projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(*projectionGL.v);

    float4x4 viewGL = GetViewMatrix(); //<-- Important to transpose!

    //Send the frustum view matrix to OpenGL
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(*viewGL.v);

    return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
    return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
    return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
    return true;
}

void ModuleCamera::WindowResized(unsigned width, unsigned height)
{
}

void ModuleCamera::SetFOV(float h_fov)
{
}

void ModuleCamera::SetAspectRation(float aspect_ratio)
{
}

void ModuleCamera::SetPlaneDistances(float near_plane, float far_plane)
{
}

void ModuleCamera::Position(float x, float y, float z)
{
}

void ModuleCamera::Orientation(float x, float y, float z)
{
}

void ModuleCamera::LookAt(float x, float y, float z)
{
}

float4x4 ModuleCamera::GetProjectionMatrix()
{
    return frustum.ProjectionMatrix().Transposed();
}

float4x4 ModuleCamera::GetViewMatrix()
{
    return float4x4(frustum.ViewMatrix()).Transposed();
}
