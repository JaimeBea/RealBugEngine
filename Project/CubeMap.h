#pragma once

#include <string>

class CubeMap
{
public:
	std::string file_names[6] = {"", "", "", "", "", ""};
	unsigned gl_texture = 0;
};