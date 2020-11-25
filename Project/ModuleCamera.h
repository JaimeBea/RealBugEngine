#pragma once

#include "Module.h"

#include "Geometry/Frustum.h"

class Model;

class ModuleCamera : public Module
{
public:
	bool Init() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetFOV(float h_fov);
	void SetAspectRatio(float aspect_ratio);
	void SetPlaneDistances(float near_plane, float far_plane);
	void SetPosition(const vec& position);
	void SetPosition(float x, float y, float z);
	void SetOrientation(const float3& orientation);
	void SetOrientation(float x, float y, float z);
	void Translate(const vec& translation);
	void Rotate(const float3x3& rotationMatrix);
	void LookAt(float x, float y, float z);
	void Focus(Model* model);

	vec GetFront() const;
	vec GetUp() const;
	vec GetWorldRight() const;
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
	float rotation_speed = 10.0f;
	float zoom_speed = 0.5f;
	float shift_multiplier = 5.0f;

private:
	Frustum frustum;
};
