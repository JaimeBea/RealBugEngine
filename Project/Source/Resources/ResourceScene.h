#pragma once

#include "Resources/Resource.h"

class ResourceScene : public Resource {
public:
	ResourceScene(UID id, const char* assetFilePath, const char* resourceFilePath);

	void BuildScene();
};
