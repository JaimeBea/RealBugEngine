#pragma once

#include "Resource.h"

#include <string>
#include <GL/glew.h>

class ResourceTexture : public Resource {
public:
	ResourceTexture(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

public:
	const GLuint glTexture = 0;
};