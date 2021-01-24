#pragma once

#include "Math/float3.h"

class Texture;

enum class ShaderType
{
	STANDARD,
	PHONG
};

class Material
{
public:
	ShaderType material_type = ShaderType::PHONG;

	// Phong

	bool has_diffuse_map = false;
	float3 diffuse_color = {0.137f, 0.263f, 0.424f};
	Texture* diffuse_map = nullptr;

	bool has_specular_map = false;
	float3 specular_color = {1.0f, 1.0f, 1.0f};
	Texture* specular_map = nullptr;

	float shininess = 300;
	bool has_shininess_in_alpha_channel = false;

	float3 ambient = {0.0f, 0.0f, 0.0f};
};