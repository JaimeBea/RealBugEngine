#pragma once
#include "Module.h"
#include "Globals.h"
#include "Geometry/Frustum.h"
#include "Math/float4x4.h"

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
	void WindowResized(unsigned width, unsigned height);

	void SetFOV(float h_fov);
	void SetAspectRation(float aspect_ratio);
	void SetPlaneDistances(float near_plane, float far_plane);
	void Position(float x, float y, float z);
	void Orientation(float x, float y, float z);
	void LookAt(float x, float y, float z);
	float4x4 GetProjectionMatrix();
	float4x4 GetViewMatrix();

private:
	Frustum frustum;
};

