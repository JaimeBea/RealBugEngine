#pragma once

#include "Module.h"

#include "MathGeoLibFwd.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/float2.h"
#include "Geometry/Frustum.h"

class Model;
class GameObject;

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
	void SetOrientation(const float3x3& rotationMatrix);
	void Translate(const vec& translation);
	void Zoom(float amount);
	void Rotate(const float3x3& rotationMatrix);
	void LookAt(float x, float y, float z);
	void Focus(const GameObject* game_object);
	void ChangeFrustrum(Frustum& frustum_, bool default_);
	void CalculateFrustumNearestObject(float2 pos);

	vec GetFront() const;
	vec GetUp() const;
	vec GetWorldRight() const;
	vec GetPosition() const;
	float3 GetOrientation() const;
	float GetFocusDistance() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;
	float GetAspectRatio() const;
	float4x4 GetProjectionMatrix() const;
	float4x4 GetViewMatrix() const;

public:
	float movement_speed = 0.4f;
	float rotation_speed = 0.2f;
	float zoom_speed = 0.001f;
	float shift_multiplier = 5.0f;
	Frustum engine_camera_frustum = Frustum();

private:
	Frustum* active_frustum = &engine_camera_frustum;
	float focus_distance = 0.0f;
};
