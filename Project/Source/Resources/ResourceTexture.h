#pragma once

#include "Resource.h"

#include <string>

class ResourceTexture : public Resource {
public:
	REGISTER_RESOURCE(ResourceTexture, ResourceType::TEXTURE);

	void Load() override;
	void Unload() override;

public:
	unsigned int glTexture = 0;
};