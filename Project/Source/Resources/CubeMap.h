#pragma once

#include <string>

class CubeMap {
public:
	std::string fileNames[6] = {"", "", "", "", "", ""};
	unsigned glTexture = 0;
};