#pragma once

#include "Component.h"
#include "ModuleCamera.h"

class ComponentCamera : public Component
{
public:
	REGISTER_COMPONENT(ComponentType::CAMERA);

	ComponentCamera(GameObject& owner);

	void Init() override;
	void Update() override;
	void DrawDebugDraw() override;
	void OnEditorUpdate() override;

	bool GetCullingStatus() const;

	Frustum frustum = Frustum();

private:
	bool activate_camera = false;
	bool frustum_culling = false;
};
