#pragma once

#include "Component.h"
#include "ModuleCamera.h"

#include "Math/float3.h"
#include "Geometry/Plane.h"

struct FrustumPlanes
{
	float3 points[8]; // 0: ftl, 1: ftr, 2: fbl, 3: fbr, 4: ntl, 5: ntr, 6: nbl, 7: nbr. (far/near, top/bottom, left/right).
	Plane planes[6]; // left, right, up, down, front, back
};

class ComponentCamera : public Component
{
public:
	REGISTER_COMPONENT(ComponentType::CAMERA);

	ComponentCamera(GameObject& owner);

	void Init() override;
	void DrawGizmos() override;
	void OnEditorUpdate() override;
	void UpdateFrustumPlanes(bool force = false);
	void Invalidate();

	bool GetCullingStatus() const;

	Frustum frustum = Frustum();
	FrustumPlanes frustum_planes = FrustumPlanes();

private:
	bool camera_selected = false;
	bool apply_frustum_culling = false;
	bool dirty = false;
};
