#pragma once

#include "Component.h"

enum class ShaderType
{
	STANDARD,
	PHONG
};

class ComponentMaterial : public Component
{
public:
	REGISTER_COMPONENT(ComponentType::MATERIAL);

	ComponentMaterial(GameObject& owner);
	void OnEditorUpdate() override;

public:
	unsigned texture = 0;
	ShaderType material_type = ShaderType::PHONG;

	// Phong
	float Kd = 1;
	float Ks = 0;
	int n = 1;
	GameObject* light = nullptr;
};
