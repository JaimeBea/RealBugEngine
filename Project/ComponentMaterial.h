#pragma once

#include "Component.h"
#include "Math/float3.h"

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
	ShaderType material_type = ShaderType::PHONG;

	// Phong
	struct Material
	{
		float3 diffuse_color = {0.0f, 0.0f, 0.0f};
		unsigned diffuse_map = 0;
		float3 specular_color = {0.0f, 0.0f, 0.0f};
		unsigned specular_map = 0;
		float shininess = 0;
		float3 ambient = {0.0f, 0.0f, 0.0f};

		bool has_diffuse_map = true;
		bool has_specular_map = false;
		bool shininess_alpha = false;
	} material;

	GameObject* light = nullptr;
};
