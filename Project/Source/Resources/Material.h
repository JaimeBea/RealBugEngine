#pragma once

#include "Math/float3.h"

class Texture;

enum class ShaderType {
	STANDARD_SPECULAR,
	STANDARD
};

class Material {
public:
	ShaderType materialType = ShaderType::STANDARD_SPECULAR;

	// Phong

	bool hasDiffuseMap = false;
	float3 diffuseColor = {0.137f, 0.263f, 0.424f};
	Texture* diffuseMap = nullptr;

	bool hasSpecularOrMetalMap = false;
	float metallic = 1.f;
	float3 specularColor = {1.0f, 1.0f, 1.0f};
	Texture* specularMap = nullptr;

	float shininess = 300;
	int hasSmoothnessInAlphaChannel = 0;

	float3 ambient = {0.0f, 0.0f, 0.0f};

	float smoothness = 0.3;
};