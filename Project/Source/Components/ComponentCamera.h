#pragma once

#include "Component.h"

#include "Geometry/Frustum.h"

class ComponentCamera : public Component {
public:
	REGISTER_COMPONENT(ComponentCamera, ComponentType::CAMERA); // Refer to ComponentType for the Constructor

	// ------- Core Functions ------ //
	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	Frustum BuildDefaultFrustum() const; // Builds a frustum object, at origin, facing Z direction, with specified FOV.

public:
	Frustum frustum = BuildDefaultFrustum(); // Frustum geometry of the camera
private:
	bool activeCamera = false;	// Indicator if this is the active camera. The active camera is the POV the scene will be rendered from.
	bool cullingCamera = false; // Indicator if this camera perfoms frustum culling. Meshes outside the frustum will not be rendered when set to true.
};
