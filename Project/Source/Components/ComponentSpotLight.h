#pragma once

#include "Component.h"

#include "Math/float3.h"

struct SpotLight
{
	float3 pos = {0.0f, 0.0f, 0.0f};
	float3 direction = {0.0f, 0.0f, 0.0f};
	float3 color = {1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;

	// Init value to rise 100 meters.
	float kc = 1.0f; //Keep in one to avoid having denominator less than 1
	float kl = 0.045f;
	float kq = 0.0075f;
	float inner_angle = pi / 12;
	float outer_angle = pi / 6;
};

class ComponentSpotLight : public Component
{
public:
	REGISTER_COMPONENT(ComponentSpotLight, ComponentType::SPOT_LIGHT);

	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue j_component) const override;
	void Load(JsonValue j_component) override;
	SpotLight& GetLightStruct() const;

private:
	SpotLight& light = SpotLight();
	bool draw_gizmos = true;
};
