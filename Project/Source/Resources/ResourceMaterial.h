#pragma once

#include "Resources/Resource.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"

#include "Math/float3.h"

class ResourceMaterial : public Resource {
private:
	// Material shader
	ResourceShader* shader = nullptr;

	// Diffuse
	float4 diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
	ResourceTexture* diffuseMap = nullptr;

	// Specular
	float4 specularColor = {1.0f, 1.0f, 1.0f, 1.0f};
	ResourceTexture* specularMap = nullptr;

	// Metalness
	ResourceTexture* metallicMap = nullptr;

	// Normal
	ResourceTexture* normalMap = nullptr;

	// Smoothness
	float smoothness = 300;
	bool hasSmoothnessInAlphaChannel = false;

public:
	ResourceMaterial(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

	ResourceShader* GetShader() {
		return shader;
	};

	float4 GetDiffuseColor() {
		return diffuseColor;
	};

	inline ResourceTexture* GetDiffuseMap() {
		return diffuseMap;
	};

	float4 GetSpecularColor() {
		return specularColor;
	};

	inline ResourceTexture* GetSpecularMap() {
		return specularMap;
	};

	inline ResourceTexture* GetMetallicMap() {
		return metallicMap;
	};

	inline ResourceTexture* GetNormalMap() {
		return normalMap;
	};

	float Getsmoothness() {
		return smoothness;
	};

	bool HasSmoothnessInAlphaChannel() {
		return hasSmoothnessInAlphaChannel;
	};
};