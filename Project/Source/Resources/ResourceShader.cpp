#include "ResourceShader.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"

ResourceShader::ResourceShader(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceShader::Load() {
	shaderProgram = App->programs->CreateProgram(this->GetAssetFilePath().c_str());
}

void ResourceShader::Unload() {
	App->programs->DeleteProgram(shaderProgram);
}
