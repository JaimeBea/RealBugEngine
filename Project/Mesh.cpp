#include "Mesh.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"

#include "assimp/mesh.h"
#include "Math/float4x4.h"
#include "GL/glew.h"

void Mesh::Load(const aiMesh* mesh)
{
	num_vertices = mesh->mNumVertices;
	num_indices = mesh->mNumFaces * 3;

	unsigned vertex_size = sizeof(float) * 3 + sizeof(float) * 2;
	unsigned vertex_buffer_size = vertex_size * num_vertices;
	unsigned position_size = sizeof(float) * 3 * num_vertices;
	unsigned uv_offset = position_size;
	unsigned uv_size = sizeof(float) * 2 * num_vertices;
	unsigned index_buffer_size = sizeof(unsigned) * num_indices;

	// Create VAO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Load VBO
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, position_size, mesh->mVertices);
	float* uv_buffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, uv_offset, uv_size, GL_MAP_WRITE_BIT);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		*(uv_buffer++) = mesh->mTextureCoords[0][i].x;
		*(uv_buffer++) = mesh->mTextureCoords[0][i].y;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, nullptr, GL_STATIC_DRAW);
	unsigned* index_buffer = (unsigned*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		// Assume triangles = 3 indices per face
		assert(mesh->mFaces[i].mNumIndices == 3);

		*(index_buffer++) = mesh->mFaces[i].mIndices[0];
		*(index_buffer++) = mesh->mFaces[i].mIndices[1];
		*(index_buffer++) = mesh->mFaces[i].mIndices[2];
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)position_size);

	// Unbind VAO
	glBindVertexArray(0);
}

void Mesh::Release()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void Mesh::Draw(const std::vector<unsigned>& materials) const
{
	unsigned program = App->program->program;
	float4x4 view = App->camera->GetViewMatrix();
	float4x4 proj = App->camera->GetProjectionMatrix();
	float4x4 model = float4x4::identity;

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj.ptr());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, materials[material_index]);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
