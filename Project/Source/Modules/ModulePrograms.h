#pragma once

#include "Module.h"

class ModulePrograms : public Module {
public:
	bool Start() override;
	bool CleanUp() override;

	void LoadShaderBinFile();
	unsigned CreateProgram(const char* shaderFile, const char* vertexSnippets = "vertex", const char* fragmentSnippets = "fragment");
	void DeleteProgram(unsigned int idProgram);

public:
	const char* filePath = "Library/shadersBin";

	// Skybox shader
	unsigned skybox = 0;

	// Ilumination Shaders
	unsigned phongSkinning = 0;
	unsigned phongNotSkinning = 0;
	unsigned standardSkinning = 0;
	unsigned standardNotSkinning = 0;
	unsigned specularSkinning = 0;
	unsigned specularNotSkinning = 0;

	//UI Shaders
	unsigned textUI = 0;
	unsigned imageUI = 0;
};
