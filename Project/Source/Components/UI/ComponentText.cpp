#include "ComponentText.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleResources.h"
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
	float buffer_data[] = {
		-0.5f,
		-0.5f,
		0.0f, //  v0 pos
		0.5f,
		-0.5f,
		0.0f, // v1 pos
		-0.5f,
		0.5f,
		0.0f, //  v2 pos

		0.5f,
		-0.5f,
		0.0f, //  v3 pos
		0.5f,
		0.5f,
		0.0f, // v4 pos
		-0.5f,
		0.5f,
		0.0f, //  v5 pos

		0.0f,
		0.0f, //  v0 texcoord
		1.0f,
		0.0f, //  v1 texcoord
		0.0f,
		1.0f, //  v2 texcoord

		1.0f,
		0.0f, //  v3 texcoord
		1.0f,
		1.0f, //  v4 texcoord
		0.0f,
		1.0f //  v5 texcoord
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
}

void ComponentText::Update() {
}

void ComponentText::OnEditorUpdate() {

	static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	ImGui::InputTextMultiline("Text input", &text, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8), flags);
	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);
	ImGui::ResourceSlot<ResourceFont>("Font", &fontID);
	
}

void ComponentText::Save(JsonValue jComponent) const {
}

void ComponentText::Load(JsonValue jComponent) {
}

void ComponentText::Draw(ComponentTransform2D* transform) {
	unsigned int program = 0;
	ResourceShader* shaderResouce = (ResourceShader*) App->resources->GetResource(shaderID);
	if (shaderResouce) {
		program = shaderResouce->GetShaderProgram();
	}
	ResourceTexture* texResource = (ResourceTexture*) App->resources->GetResource(textureID);
	if (texResource == nullptr) return;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
	glUseProgram(program);

	float4x4 modelMatrix = transform->GetGlobalMatrixWithSize();
	float4x4* proj = &float4x4::D3DOrthoProjLH(-1, 1, App->renderer->viewportWidth, App->renderer->viewportHeight); //near plane. far plane, screen width, screen height

	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	glUniform4fv(glGetUniformLocation(program, "inputColor"), 1, color.ptr());

	//std::string font = "";
	std::vector<Character> characters;
	App->userInterface->GetCharactersInString(fontID, text, characters);

	// Iterate through all characters

	//for (Character character : characters){
	//
	//	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	//	glBindTexture(GL_TEXTURE_2D, character.textureID);
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//}

	for (char c : text) {
		Character character = App->userInterface->GetCharacter(fontID, c);
		glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
		glBindTexture(GL_TEXTURE_2D, character.textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void ComponentText::SetText(const std::string& newText) {
	text = newText;
}

void ComponentText::SetFont() {
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
