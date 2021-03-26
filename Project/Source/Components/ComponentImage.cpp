#include "ComponentImage.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"

ComponentImage::~ComponentImage() {
	DestroyVBO();
}

void ComponentImage::Init() {
	CreateVBO();
}

void ComponentImage::Update() {

}

void ComponentImage::OnEditorUpdate() {
	if (ImGui::CollapsingHeader("Image")) {
		ImGui::TextColored(App->editor->textColor, "Texture Settings:");

		std::vector<Texture*> textures;
		for (Texture& texture : App->resources->textures) textures.push_back(&texture);

		ImGui::ColorEdit4("Color##", color.ptr());

		std::string& currentDiffuseTexture = texture->fileName;
		if (ImGui::BeginCombo("Texture##", currentDiffuseTexture.c_str())) {
			for (unsigned i = 0; i < textures.size(); ++i) {
				bool isSelected = (currentDiffuseTexture == textures[i]->fileName);
				if (ImGui::Selectable(textures[i]->fileName.c_str(), isSelected)) {
					texture = textures[i];
				};
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		
		ImGui::Checkbox("Alpha transparency", &alphaTransparency);

		ImGui::Text("");

		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Texture Preview");
		ImGui::TextWrapped("Size:");
		ImGui::SameLine();
		int width;
		int height;
		glGetTextureLevelParameteriv(texture->glTexture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(texture->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
		ImGui::TextWrapped("%d x %d", width, height);
		ImGui::Image((void*) texture->glTexture, ImVec2(200, 200));
		ImGui::Separator();
	}
}

void ComponentImage::CreateVBO() {


	//float buffer_data[] = {
	//	 0.0f, 0.0f, 0.0f, //  v0 pos
	//	 1.0f, 0.0f, 0.0f, // v1 pos
	//	 0.0f, 1.0f, 0.0f, //  v2 pos

	//	 1.0f, 0.0f, 0.0f, //  v3 pos
	//	 1.0f, 1.0f, 0.0f, // v4 pos
	//	 0.0f, 1.0f, 0.0f, //  v5 pos

	//	0.0f, 0.0f, //  v0 texcoord
	//	1.0f, 0.0f, //  v1 texcoord
	//	0.0f, 1.0f, //  v2 texcoord

	//	1.0f, 0.0f, //  v3 texcoord
	//	1.0f, 1.0f, //  v4 texcoord
	//	0.0f, 1.0f //  v5 texcoord
	//};

	// centered position
	float buffer_data[] = {
		-0.5f, -0.5f, 0.0f, //  v0 pos
		 0.5f, -0.5f, 0.0f, // v1 pos
		-0.5f,  0.5f, 0.0f, //  v2 pos

		 0.5f, -0.5f, 0.0f, //  v3 pos
		 0.5f, 0.5f, 0.0f, // v4 pos
		-0.5f, 0.5f, 0.0f, //  v5 pos

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

void ComponentImage::Draw(ComponentTransform2D* transform) {

	if (alphaTransparency) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	unsigned int program = App->programs->uiProgram;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
	glUseProgram(program);

	float4x4 modelMatrix = transform->GetGlobalMatrix();
	float4x4* proj = &float4x4::D3DOrthoProjLH(-1, 1, App->renderer->viewportWidth, App->renderer->viewportHeight); //near plane. far plane, screen width, screen height

	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	glUniform4fv(glGetUniformLocation(program, "inputColor"), 1, color.ptr());


	glBindTexture(GL_TEXTURE_2D, texture->glTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_BLEND);
}

void ComponentImage::SetTexture(Texture* text) {
	texture = text;
}
