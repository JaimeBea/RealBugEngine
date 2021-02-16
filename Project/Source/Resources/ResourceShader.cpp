#include "ResourceShader.h"

ResourceShader::ResourceShader(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceShader::Load() {
}

void ResourceShader::Unload() {
}
