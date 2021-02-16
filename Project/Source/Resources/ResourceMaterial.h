#pragma once

#include "Resources/Resource.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"

#include "Math/float3.h"

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(UID id, std::string localFilePath);

	bool Import() override;
	void Delete() override;

public:
	ResourceShader* shaderType = nullptr;

	// Phong

	bool hasDiffuseMap = false;
	float3 diffuseColor = {0.137f, 0.263f, 0.424f};
	ResourceTexture* diffuseMap = nullptr;

	bool hasSpecularMap = false;
	float3 specularColor = {1.0f, 1.0f, 1.0f};
	ResourceTexture* specularMap = nullptr;

	float shininess = 300;
	bool hasShininessInAlphaChannel = false;

	float3 ambient = {0.0f, 0.0f, 0.0f};

protected:
	void Load() override;
	void Unload() override;
};