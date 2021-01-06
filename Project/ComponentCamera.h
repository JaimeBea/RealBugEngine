#pragma once

#include "Component.h"
#include "ModuleCamera.h"

class ComponentCamera : public Component
{
public:
	REGISTER_COMPONENT(ComponentType::CAMERA);

	ComponentCamera(GameObject& owner);

	void Init() override;
	void DrawGizmos() override;
	void OnEditorUpdate() override;

	bool GetCullingStatus() const;

	Frustum frustum = Frustum();

private:
	bool camera_selected = false;
	bool apply_frustum_culling = false;
};
