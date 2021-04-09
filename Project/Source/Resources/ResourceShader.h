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

	void SaveShaderType() const;

	unsigned int GetShaderProgram() {
		return shaderProgram;
	}
	ShaderType GetShaderType() {
		return shaderType;
	}
	void SetShaderType(const ShaderType type) {
		shaderType = type;
	}

private:
	unsigned int shaderProgram = 0;
	ShaderType shaderType = ShaderType::PHONG;
};
