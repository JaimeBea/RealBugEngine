#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID id, std::string localFilePath) : Resource(id, localFilePath) {}

bool ResourceMaterial::Import() {
	return true;
}

void ResourceMaterial::Delete() {
}

void ResourceMaterial::Load() {
}

void ResourceMaterial::Unload() {
}
