#include "ComponentText.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleEditor.h"
#include "Resources/ResourceTexture.h""
#include "Resources/ResourceShader.h"
#include "Resources/ResourceFont.h"
#include "GL/glew.h"
#include "Math/TransformOps.h"
#include "FileSystem/JsonValue.h"
#include "Utils/Logging.h"
#include "Utils/ImGuiUtils.h"
#include "imgui_stdlib.h"

#define JSON_TAG_TEXTURE_SHADERID "ShaderID"
#define JSON_TAG_TEXTURE_TEXTUREID "TextureID"
#define JSON_TAG_COLOR "Color"
#define JSON_TAG_ALPHATRANSPARENCY "AlphaTransparency"

ComponentText::~ComponentText() {
	glDeleteBuffers(1, &vbo);
}

void ComponentText::Init() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentText::Update() {
}

void ComponentText::OnEditorUpdate() {

	static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	ImGui::InputTextMultiline("Text input", &text, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8), flags);
	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);
	ImGui::ResourceSlot<ResourceFont>("Font", &fontID);
	ImGui::DragFloat("Font Size", &fontSize, 0.2f, 0, FLT_MAX);
	ImGui::ColorEdit4("Color##", color.ptr());
}

void ComponentText::Save(JsonValue jComponent) const {
}

void ComponentText::Load(JsonValue jComponent) {
}

void ComponentText::Draw(ComponentTransform2D* transform) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int program = 0;
	ResourceShader* shaderResouce = (ResourceShader*) App->resources->GetResource(shaderID);
	if (shaderResouce) {
		program = shaderResouce->GetShaderProgram();
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	glUseProgram(program);

	float4x4 modelMatrix;
	float4x4* proj = &App->camera->GetProjectionMatrix();

	if (App->time->IsGameRunning() || App->editor->panelScene.IsUsing2D()) {
		proj = &float4x4::D3DOrthoProjLH(-1, 1, App->renderer->viewportWidth, App->renderer->viewportHeight); //near plane. far plane, screen width, screen height
		float4x4 view = float4x4::identity;
		modelMatrix = transform->GetGlobalMatrixWithSize();

		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view.ptr());
	} else {
		float4x4* view = &App->camera->GetViewMatrix();
		modelMatrix = transform->GetGlobalMatrixWithSize(true);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view->ptr());
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj->ptr());
	glUniform4fv(glGetUniformLocation(program, "textColor"), 1, color.ptr());

	float3 position = transform->GetPosition();

	float x = position.x;
	float y = position.y;
	// FontSize / size of imported font
	float scale = fontSize / 48.0f;

	for (char c : text) {
		Character character = App->userInterface->GetCharacter(fontID, c);

		float xpos = x + character.bearing.x * scale;
		float ypos = y - (character.size.y - character.bearing.y) * scale;

		float w = character.size.x * scale;
		float h = character.size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos, ypos, 0.0f, 1.0f},
			{xpos + w, ypos, 1.0f, 1.0f},

			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f}};

		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, character.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
}

void ComponentText::SetText(const std::string& newText) {
	text = newText;
}

void ComponentText::SetFontSize(float newfontSize) {
	fontSize = newfontSize;
}

void ComponentText::SetFontColor(const float4& newColor) {
	color = newColor;
}

float4 ComponentText::GetFontColor() const {
	return color;
}

