#pragma once

#include "UID.h"

class CubeMap
{
public:
	UID file_ids[6] = {0, 0, 0, 0, 0, 0};
	unsigned gl_texture = 0;
};