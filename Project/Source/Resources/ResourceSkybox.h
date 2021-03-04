#pragma once

#include "Resource.h"

#include <string>
class ResourceSkybox : public Resource {
	ResourceSkybox(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

public:
	unsigned glTexture = 0;
};