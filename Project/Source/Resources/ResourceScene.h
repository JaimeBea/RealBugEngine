#pragma once

#include "Resources/Resource.h"

class ResourceScene : public Resource {
public:
	ResourceScene(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;
};
