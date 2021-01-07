#pragma once

#include <string>

class Mesh
{
public:
	std::string file_name = "";
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned num_vertices = 0;
	unsigned num_indices = 0;
	unsigned material_index = 0;
};