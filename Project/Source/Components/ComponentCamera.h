#pragma once

#include "Component.h"

#include "Geometry/Frustum.h"

class ComponentCamera : public Component {
public:
	REGISTER_COMPONENT(ComponentCamera, ComponentType::CAMERA);

	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	Frustum BuildDefaultFrustum() const;

public:
	Frustum frustum = BuildDefaultFrustum();

private:
	bool activeCamera = false;
	bool cullingCamera = false;
};
