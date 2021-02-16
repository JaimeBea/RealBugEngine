#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceMesh::Load() {
}

void ResourceMesh::Unload() {
}
