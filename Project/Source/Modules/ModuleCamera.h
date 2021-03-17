#pragma once

#include "Module.h"
#include "Utils/Quadtree.h"

#include "MathGeoLibFwd.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include <vector>

class Model;
class GameObject;

struct FrustumPlanes {
	float3 points[8]; // 0: ftl, 1: ftr, 2: fbl, 3: fbr, 4: ntl, 5: ntr, 6: nbl, 7: nbr. (far/near, top/bottom, left/right).
	Plane planes[6]; // left, right, up, down, front, back
};

class ModuleCamera : public Module {
public:
	bool Init() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetFOV(float hFov);
	void SetAspectRatio(float aspectRatio);
	void SetPlaneDistances(float nearPlane, float farPlane);
	void SetPosition(const vec& position);
	void SetPosition(float x, float y, float z);
	void SetOrientation(const float3x3& rotationMatrix);
	void Translate(const vec& translation);
	void Zoom(float amount);
	void Rotate(const float3x3& rotationMatrix);
	void LookAt(float x, float y, float z);
	void Focus(const GameObject* gameObject);
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
	void EnableOrtographic();
	void EnablePerspective();

public:
	float movementSpeed = 0.4f;
	float rotationSpeed = 0.2f;
	float zoomSpeed = 0.001f;
	float shiftMultiplier = 5.0f;
	Frustum engineCameraFrustum = Frustum();
	Frustum editorCameraFrustum = Frustum();

private:
	void GetIntersectingAABBRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& nodeAABB, const LineSegment& ray, std::vector<GameObject*>& intersectingObjects);

private:
	float focusDistance = 0.0f;

	Frustum* activeFrustum = &engineCameraFrustum;
	Frustum* cullingFrustum = &engineCameraFrustum;
	Frustum* editorFrustum = &editorCameraFrustum;

	FrustumPlanes frustumPlanes = FrustumPlanes();
};
