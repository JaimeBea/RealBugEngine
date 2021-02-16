#include "ResourceScene.h"

ResourceScene::ResourceScene(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceScene::Load() {
}

void ResourceScene::Unload() {
}
