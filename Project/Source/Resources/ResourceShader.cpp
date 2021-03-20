#include "ResourceShader.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"

void ResourceShader::Load() {
	shaderProgram = App->programs->CreateProgram(this->GetAssetFilePath().c_str());
}

void ResourceShader::Unload() {
	App->programs->DeleteProgram(shaderProgram);
}
