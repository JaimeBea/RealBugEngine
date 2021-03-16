#pragma once

#include "Resources/Resource.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"

#include "Math/float3.h"

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

	void SaveToFile(const char* filePath);

public:
	// Material shader
	ResourceShader* shader = nullptr;

	// Diffuse
	bool hasDiffuseMap = false;
	float4 diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
	ResourceTexture* diffuseMap = nullptr;

	// Specular
	bool hasSpecularMap = false;
	float4 specularColor = {1.0f, 1.0f, 1.0f, 1.0f};
	ResourceTexture* specularMap = nullptr;

	// Metalness
	ResourceTexture* metallicMap = nullptr;

	// Normal
	ResourceTexture* normalMap = nullptr;

	// Smoothness
	float smoothness = 300;
	bool hasSmoothnessInAlphaChannel = false;
};