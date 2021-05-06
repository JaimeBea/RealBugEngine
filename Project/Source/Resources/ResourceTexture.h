#pragma once

#include "Resource.h"

class ResourceTexture : public Resource {
public:
	REGISTER_RESOURCE(ResourceTexture, ResourceType::TEXTURE);

	void Load() override;
	void Unload() override;
	void OnEditorUpdate() override;

public:
	unsigned int glTexture = 0;
};