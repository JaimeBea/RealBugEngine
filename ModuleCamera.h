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
	void Rotate(float x, float y, float z);
	void Rotate(Quat rotation);
	void LookAt(float x, float y, float z);
	float4x4 GetProjectionMatrix();
	float4x4 GetViewMatrix();

private:
	Frustum frustum;
};

