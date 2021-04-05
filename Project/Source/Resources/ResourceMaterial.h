#pragma once

#include "Resources/Resource.h"

#include "Math/float3.h"

class ResourceMaterial : public Resource {
public:
	REGISTER_RESOURCE(ResourceMaterial, ResourceType::MATERIAL);

	void Load() override;
	void Unload() override;

	void SaveToFile(const char* filePath);

public:
	// Material shader
	UID shaderId = 0;

	// Diffuse
	bool hasDiffuseMap = false;
	float4 diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
	UID diffuseMapId = 0;

	// Specular
	bool hasSpecularMap = false;
	float4 specularColor = {1.0f, 1.0f, 1.0f, 1.0f};
	UID specularMapId = 0;

	// Metalness
	UID metallicMapId = 0;

	// Normal
	UID normalMapId = 0;

	// Smoothness
	float smoothness = 300;
	bool hasSmoothnessInAlphaChannel = false;
};