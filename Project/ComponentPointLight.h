#pragma once

#include "Component.h"

#include "Math/float3.h"

struct PointLight
{
	float3 direction = {0.0f, 0.0f, 0.0f};
	float3 color = {1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;

	// Init value to rise 100 meters.
	float kc = 1.0f; //Keep in one to avoid having denominator less than 1
	float kl = 0.045f;
	float kq = 0.0075;
};

class ComponentPointLight : public Component
{
public:
	REGISTER_COMPONENT(ComponentPointLight, ComponentType::POINT_LIGHT);

	void DrawGizmos() override;
	void OnEditorUpdate() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;
	PointLight GetLightStruct() const;

private:
	PointLight light = PointLight();
};
