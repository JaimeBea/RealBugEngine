#pragma once

#include "UID.h"

class Mesh
{
public:
	UID file_id = 0;
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned num_vertices = 0;
	unsigned num_indices = 0;
	unsigned material_index = 0;
};