#pragma once

#include "Component.h"

#include "Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:
	REGISTER_COMPONENT(ComponentCamera, ComponentType::CAMERA);

	void Init() override;
	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

	Frustum frustum = Frustum();

private:
	bool active_camera = false;
	bool culling_camera = false;
};
