#include "ResourceScript.h"

ResourceScript::ResourceScript(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceScript::Load() {
}

void ResourceScript::Unload() {
}