#pragma once
#include "Resources/Resource.h"

class ResourceScript : public Resource {
public:
	ResourceScript(UID id, const char* assetFilePath, const char* resourceFilePath);
	void Load() override;
	void Unload() override;
};