#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceMaterial::Load() {
}

void ResourceMaterial::Unload() {
}
