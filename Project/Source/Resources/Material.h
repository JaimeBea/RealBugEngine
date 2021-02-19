#pragma once

#include "Math/float3.h"

class Texture;

enum class ShaderType {
	STANDARD,
	PHONG
};

class Material {
public:
	ShaderType materialType = ShaderType::PHONG;

	// Phong

	bool hasDiffuseMap = false;
	float3 diffuseColor = {0.137f, 0.263f, 0.424f};
	Texture* diffuseMap = nullptr;

	bool hasSpecularMap = false;
	float3 specularColor = {1.0f, 1.0f, 1.0f};
	Texture* specularMap = nullptr;

	float shininess = 300;
	int hasShininessInAlphaChannel = 0;

	float3 ambient = {0.0f, 0.0f, 0.0f};

	float smoothness = 0.1;
};