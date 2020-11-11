#pragma once

#include "Module.h"
#include "Geometry/Frustum.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void WindowResized(int width, int height);

	void SetFOV(float h_fov);
	void SetAspectRatio(float aspect_ratio);
	void SetPlaneDistances(float near_plane, float far_plane);
	void SetPosition(vec position);
	void SetPosition(float x, float y, float z);
	void SetOrientation(float3 orientation);
	void SetOrientation(float x, float y, float z);
	void Translate(vec translation);
	void Rotate(float3x3 rotationMatrix);
	void LookAt(float x, float y, float z);

	vec GetFront() const;
	vec GetUp() const;
	vec GetPosition() const;
	float3 GetOrientation() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;
	float GetAspectRatio() const;
	float4x4 GetProjectionMatrix() const;
	float4x4 GetViewMatrix() const;

public:
	float movement_speed = 4.0f;
	float rotation_speed = 90.0f;
	float zoom_speed = 4.0f;

private:
	Frustum frustum;
};
