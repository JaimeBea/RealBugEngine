#include "ResourceShader.h"

ResourceShader::ResourceShader(UID id, std::string localFilePath) : Resource(id, localFilePath) {}

bool ResourceShader::Import() {
	return true;
}

void ResourceShader::Delete() {
}

void ResourceShader::Load() {
}

void ResourceShader::Unload() {
}
