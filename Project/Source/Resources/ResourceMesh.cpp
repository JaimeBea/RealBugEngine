#include "ResourceMesh.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/MSTimer.h"
#include "Modules/ModuleFiles.h"

#include "GL/glew.h"

#include "Utils/Leaks.h"

void ResourceMesh::Load() {
	// Timer to measure loading a mesh
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Loading mesh from path: \"%s\".", filePath.c_str());

	// Load file
	Buffer<char> buffer = App->files->Load(filePath.c_str());
	char* cursor = buffer.Data();

	// Header
	numVertices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);
	numIndices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	unsigned positionSize = sizeof(float) * 3;
	unsigned normalSize = sizeof(float) * 3;
	unsigned uvSize = sizeof(float) * 2;
	unsigned indexSize = sizeof(unsigned);

	unsigned vertexSize = positionSize + normalSize + uvSize;
	unsigned vertexBufferSize = vertexSize * numVertices;
	unsigned indexBufferSize = indexSize * numIndices;

	// Vertices
	float* vertices = (float*) cursor;
	cursor += vertexSize * numVertices;

	// Indices
	unsigned* indices = (unsigned*) cursor;

	LOG("Loading %i vertices...", numVertices);

	// Create VAO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Load VBO
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices, GL_STATIC_DRAW);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indices, GL_STATIC_DRAW);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) positionSize);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*) (positionSize + normalSize));

	// Unbind VAO
	glBindVertexArray(0);

	unsigned timeMs = timer.Stop();
	LOG("Mesh loaded in %ums", timeMs);
}

void ResourceMesh::Unload() {
	if (!vao) return;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

std::vector<Triangle> ResourceMesh::ExtractTriangles(const float4x4& modelMatrix) const {
	std::string filePath = GetResourceFilePath();

	// Load file
	Buffer<char> buffer = App->files->Load(filePath.c_str());
	char* cursor = buffer.Data();

	// Header
	unsigned numVertices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);
	unsigned numIndices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	// Vertices
	std::vector<float3> vertices;
	for (unsigned i = 0; i < numVertices; i++) {
		float vertex[3] = {};
		vertex[0] = *((float*) cursor);
		cursor += sizeof(float);
		vertex[1] = *((float*) cursor);
		cursor += sizeof(float);
		vertex[2] = *((float*) cursor);
		cursor += sizeof(float) * 6;
		vertices.push_back((modelMatrix * float4(vertex[0], vertex[1], vertex[2], 1)).xyz());
	}

	std::vector<Triangle> triangles;
	triangles.reserve(numIndices / 3);
	for (unsigned i = 0; i < numIndices / 3; i++) {
		unsigned triangeIndices[3] = {};
		triangeIndices[0] = *((unsigned*) cursor);
		cursor += sizeof(unsigned);
		triangeIndices[1] = *((unsigned*) cursor);
		cursor += sizeof(unsigned);
		triangeIndices[2] = *((unsigned*) cursor);
		cursor += sizeof(unsigned);
		triangles.push_back(Triangle(vertices[triangeIndices[0]], vertices[triangeIndices[1]], vertices[triangeIndices[2]]));
	}

	return triangles;
}
