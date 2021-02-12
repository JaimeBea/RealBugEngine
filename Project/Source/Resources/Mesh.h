#pragma once

#include <string>

class Mesh {
public:
	std::string fileName = "";
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned materialIndex = 0;
};