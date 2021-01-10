#pragma once

#include "Component.h"

#include "Math/float3.h"

class ComponentLight : public Component
{
public:
	REGISTER_COMPONENT(ComponentLight, ComponentType::LIGHT);

	void OnEditorUpdate() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

public:
	struct Light
	{
		float3 position = {1.0f, 1.0f, 1.0f};
		float3 color = {1.0f, 1.0f, 1.0f};
	} light;
};
