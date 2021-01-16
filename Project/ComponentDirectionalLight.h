#pragma once

#include "Component.h"

#include "Math/float3.h"

struct DirectionalLight
{
	float3 direction = {0.0f, 0.0f, 0.0f};
	float3 color = {1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;
};

class ComponentDirectionalLight : public Component
{
public:
	REGISTER_COMPONENT(ComponentDirectionalLight, ComponentType::DIRECTIONAL_LIGHT);

	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;
	DirectionalLight& GetLightStruct() const;

private:
	DirectionalLight& light = DirectionalLight();
	bool draw_gizmos = true;
};
