#pragma once

#include "Module.h"

#include "MathGeoLibFwd.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"

class Model;
class GameObject;

struct FrustumPlanes
{
	float3 points[8]; // 0: ftl, 1: ftr, 2: fbl, 3: fbr, 4: ntl, 5: ntr, 6: nbl, 7: nbr. (far/near, top/bottom, left/right).
	Plane planes[6]; // left, right, up, down, front, back
};

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
	void CalculateFrustumNearestObject(float2 pos);
	void ChangeActiveFrustum(Frustum& frustum, bool change);
	void ChangeCullingFrustum(Frustum& frustum, bool change);
	void CalculateFrustumPlanes();

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
	Frustum GetEngineFrustum() const;
	Frustum* GetActiveFrustum() const;
	Frustum* GetCullingFrustum() const;
	const FrustumPlanes& GetFrustumPlanes() const;

public:
	float movement_speed = 0.4f;
	float rotation_speed = 0.2f;
	float zoom_speed = 0.001f;
	float shift_multiplier = 5.0f;
	Frustum engine_camera_frustum = Frustum();

private:
	float focus_distance = 0.0f;

	Frustum* active_frustum = &engine_camera_frustum;
	Frustum* culling_frustum = &engine_camera_frustum;

	FrustumPlanes frustum_planes = FrustumPlanes();
};
