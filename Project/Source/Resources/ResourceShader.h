#pragma once

#include "Resources/Resource.h"

enum class ShaderType { PHONG };

class ResourceShader : public Resource {
public:
	REGISTER_RESOURCE(ResourceShader, ResourceType::SHADER);

	void Load() override;
	void Unload() override;

	unsigned int GetShaderProgram() {
		return shaderProgram;
	}
	ShaderType GetShaderType() {
		return shaderType; 
	}

private:
	unsigned int shaderProgram = 0;
	ShaderType shaderType = ShaderType::PHONG;
};
