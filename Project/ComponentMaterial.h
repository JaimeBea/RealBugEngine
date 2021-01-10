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
	REGISTER_COMPONENT(ComponentMaterial, ComponentType::MATERIAL);

	void OnEditorUpdate() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

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
