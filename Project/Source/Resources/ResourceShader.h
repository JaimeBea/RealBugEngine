#pragma once

#include "Resources/Resource.h"
enum class ShaderType { PHONG };


class ResourceShader : public Resource {
public:
	ResourceShader(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

	unsigned int GetShaderProgram() {
		return shaderProgram;
	}
	ShaderType GetShaderType() {
		return shaderType; 
	}

private:
	unsigned int shaderProgram;
	ShaderType shaderType;
};
