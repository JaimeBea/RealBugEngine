#pragma once

#include "Component.h"

#include "Math/float3.h"

enum class LightType {
	DIRECTIONAL,
	POINT,
	SPOT
};

class ComponentLight : public Component {
public:
	REGISTER_COMPONENT(ComponentLight, ComponentType::LIGHT);

	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

public:
	bool drawGizmos = true;

	LightType lightType = LightType::DIRECTIONAL;

	float3 pos = {0.0f, 0.0f, 0.0f};
	float3 direction = {0.0f, 0.0f, 0.0f};
	float3 color = {1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;

	// Init value to rise 100 meters.
	float kc = 1.0f; //Keep in one to avoid having denominator less than 1
	float kl = 0.045f;
	float kq = 0.0075f;
	float innerAngle = pi / 12;
	float outerAngle = pi / 6;
};
