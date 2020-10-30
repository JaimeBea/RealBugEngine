#pragma once
#include "Module.h"
#include "Globals.h"
#include "Geometry/Frustum.h"
#include "Math/float4x4.h"
#include "SDL_types.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(Sint32 width, Sint32 height);

	void SetFOV(float h_fov);
	void SetAspectRatio(float aspect_ratio);
	void SetPlaneDistances(float near_plane, float far_plane);
	void SetPosition(float x, float y, float z);
	void SetOrientation(float x, float y, float z);
	void Translate(vec translation);
	void Rotate(float3x3 rotationMatrix);
	void LookAt(float x, float y, float z);
	float4x4 GetProjectionMatrix();
	float4x4 GetViewMatrix();

private:
	Frustum frustum;
	float movement_speed = 4.0f;
	float rotation_speed = 90.0f;
};

