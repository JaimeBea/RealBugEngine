#pragma once

#include "Component.h"

#include "Math/float3.h"

class Texture;

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
	GameObject* light = nullptr;

	ShaderType material_type = ShaderType::PHONG;

	// Phong

	bool has_diffuse_map = false;
	float3 diffuse_color = {0.0f, 0.0f, 0.0f};
	Texture* diffuse_map = nullptr;

	bool has_specular_map = false;
	float3 specular_color = {0.0f, 0.0f, 0.0f};
	Texture* specular_map = nullptr;

	float shininess = 0;
	bool has_shininess_in_alpha_channel = false;

	float3 ambient = {0.0f, 0.0f, 0.0f};
};
