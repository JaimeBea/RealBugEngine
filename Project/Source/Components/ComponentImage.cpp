#include "ComponentImage.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "GL/glew.h"

ComponentImage::~ComponentImage() {
	DestroyVBO();
}

void ComponentImage::Init() {
	CreateVBO();
}

void ComponentImage::Update() {

}

void ComponentImage::CreateVBO() {


	float buffer_data[] = {
		-0.5f, 0.0f, 0.0f, //  v0 pos
		 0.5f, 0.0f, 0.0f, // v1 pos
		-0.5f, 1.0f, 0.0f, //  v2 pos

		 0.5f, 0.0f, 0.0f, //  v3 pos
		 0.5f, 1.0f, 0.0f, // v4 pos
		-0.5f, 1.0f, 0.0f, //  v5 pos

		0.0f, 0.0f, //  v0 texcoord
		1.0f, 0.0f, //  v1 texcoord
		0.0f, 1.0f, //  v2 texcoord

		1.0f, 0.0f, //  v3 texcoord
		1.0f, 1.0f, //  v4 texcoord
		0.0f, 1.0f //  v5 texcoord
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
}

void ComponentImage::DestroyVBO() {
	glDeleteBuffers(1, &vbo);
}

void ComponentImage::Draw(const float4x4& modelMatrix) {

	unsigned int program = App->programs->uiProgram;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
	glUseProgram(program);

	float4x4* proj = &float4x4::D3DOrthoProjLH(-1, 200, 1080.0f, 720.0f); //near plane. far plane, screen width, screen height
	float4x4 *view = &App->camera->GetProjectionMatrix();

	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	glBindTexture(GL_TEXTURE_2D, texture->glTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ComponentImage::SetTexture(Texture* text) {
	texture = text;
}
