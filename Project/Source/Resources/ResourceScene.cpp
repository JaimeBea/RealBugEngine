#include "ResourceScene.h"

ResourceScene::ResourceScene(UID id, std::string localFilePath) : Resource(id, localFilePath) {}

bool ResourceScene::Import() {
	return true;
}

void ResourceScene::Delete() {
}

void ResourceScene::Load() {
}

void ResourceScene::Unload() {
}
