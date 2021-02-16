#pragma once

#include "Resources/Resource.h"

class ResourceShader : public Resource {
public:
	ResourceShader(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;
};
