#pragma once

#include "Component.h"

#include "Math/float3.h"

class ComponentLight : public Component
{
public:
	static const ComponentType static_type = ComponentType::LIGHT;

	ComponentLight(GameObject& owner);
	void OnEditor() override;

public:

	float3 light_color = { 1.0f, 1.0f, 1.0f };
	float3 light_direction = { -1.0f, -1.0f, -1.0f };
};

