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

	// Diffuse
	float4 diffuseColor = {1.f, 1.f, 1.f, 1.0f};
	Texture* diffuseMap = nullptr;

	// Metalness
	float metallic = 1.f;
	Texture* metallicMap = nullptr;

	// Specular
	float4 specularColor = {0.0f, 0.0f, 0.0f, 1.0f};
	Texture* specularMap = nullptr;

	// Normal
	Texture* normalMap = nullptr;

	// Smoothness
	float smoothness = 0.3;
	int hasSmoothnessInAlphaChannel = 0;
};