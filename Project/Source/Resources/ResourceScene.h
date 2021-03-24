#pragma once

#include "Resources/Resource.h"

class ResourceScene : public Resource {
public:
	REGISTER_RESOURCE(ResourceScene, ResourceType::SCENE);

	void BuildScene();
};
