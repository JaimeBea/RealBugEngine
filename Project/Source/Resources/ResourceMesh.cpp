#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(UID id, std::string localFilePath) : Resource(id, localFilePath) {}

bool ResourceMesh::Import() {
	return true;
}

void ResourceMesh::Delete() {
}

void ResourceMesh::Load() {
}

void ResourceMesh::Unload() {
}
