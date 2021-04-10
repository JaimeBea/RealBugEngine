#pragma once

#include "Resources/Resource.h"

enum class ShaderType {
	PHONG,
	STANDARD_SPECULAR,
	STANDARD
};

class ResourceShader : public Resource {
public:
	REGISTER_RESOURCE(ResourceShader, ResourceType::SHADER);

	void Load() override;
	void Unload() override;

	ShaderType GetShaderType();

	void SetShaderType(const ShaderType type);

	void SaveShaderType() const;

	unsigned int GetShaderProgram() {
		return shaderProgram;
	}


private:
	unsigned int shaderProgram = 0;
	ShaderType shaderType = ShaderType::PHONG;
};
