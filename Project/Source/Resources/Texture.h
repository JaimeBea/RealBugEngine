#pragma once

#include <string>

enum TextureType {
	DIFFUSE_MAP,
	SPECULAR_MAP,
	NORMAL_MAP
};

class Texture {
public:
	std::string fileName = "";
	unsigned glTexture = 0;
};