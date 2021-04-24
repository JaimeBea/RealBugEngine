#include "ComponentText.h"

#include "GameObject.h"
#include "Application.h"
#include "ComponentTransform2D.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleEditor.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceFont.h"
#include "FileSystem/JsonValue.h"

#include "GL/glew.h"
#include "Math/TransformOps.h"
#include "Utils/Logging.h"
#include "Utils/ImGuiUtils.h"
#include "imgui_stdlib.h"

#include "Utils/Leaks.h"

#define JSON_TAG_TEXT_SHADERID "ShaderID"
#define JSON_TAG_TEXT_FONTID "FontID"
#define JSON_TAG_TEXT_FONTSIZE "FontSize"
#define JSON_TAG_TEXT_VALUE "Value"
#define JSON_TAG_COLOR "Color"

ComponentText::~ComponentText() {
	//TODO DECREASE REFERENCE COUNT OF SHADER AND FONT, MAYBE IN A NEW COMPONENT::CLEANUP?
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

void ComponentText::OnEditorUpdate() {
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	if (ImGui::InputTextMultiline("Text input", &text, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8), flags)) {
		SetText(text);
	}
	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);
	UID oldFontID = fontID;
	ImGui::ResourceSlot<ResourceFont>("Font", &fontID);
	if (oldFontID != fontID) {
		RecalculcateVertices();
	}

	if (ImGui::DragFloat("Font Size", &fontSize, 0.2f, 0, FLT_MAX)) {
		RecalculcateVertices();
	}
	ImGui::ColorEdit4("Color##", color.ptr());
}

void ComponentText::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_TEXT_SHADERID] = shaderID;
	jComponent[JSON_TAG_TEXT_FONTID] = fontID;
	jComponent[JSON_TAG_TEXT_FONTSIZE] = fontSize;
	jComponent[JSON_TAG_TEXT_VALUE] = text.c_str();

	JsonValue jColor = jComponent[JSON_TAG_COLOR];
	jColor[0] = color.x;
	jColor[1] = color.y;
	jColor[2] = color.z;
	jColor[3] = color.w;
}

void ComponentText::Load(JsonValue jComponent) {
	shaderID = jComponent[JSON_TAG_TEXT_SHADERID];
	App->resources->IncreaseReferenceCount(shaderID);

	fontID = jComponent[JSON_TAG_TEXT_FONTID];
	App->resources->IncreaseReferenceCount(fontID);

	fontSize = jComponent[JSON_TAG_TEXT_FONTSIZE];

	text = jComponent[JSON_TAG_TEXT_VALUE];

	JsonValue jColor = jComponent[JSON_TAG_COLOR];
	color.Set(jColor[0], jColor[1], jColor[2], jColor[3]);
}

void ComponentText::DuplicateComponent(GameObject& owner) {
	ComponentText* component = owner.CreateComponent<ComponentText>();
	component->shaderID = shaderID;
	component->fontID = fontID;
	component->fontSize = fontSize;
	component->text = text;
	component->color = color;

	if (shaderID != 0) {
		App->resources->IncreaseReferenceCount(shaderID);
	}
	if (fontID != 0) {
		App->resources->IncreaseReferenceCount(fontID);
	}
}

void ComponentText::Draw(ComponentTransform2D* transform) const {
	if (fontID == 0 || shaderID == 0) {
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int program = 0;
	ResourceShader* shaderResouce = App->resources->GetResource<ResourceShader>(shaderID);
	if (shaderResouce) {
		program = shaderResouce->GetShaderProgram();
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	glUseProgram(program);

	float4x4& proj = App->camera->GetProjectionMatrix();

	if (App->time->IsGameRunning() || App->editor->panelScene.IsUsing2D()) {
		proj = float4x4::D3DOrthoProjLH(-1, 1, App->renderer->viewportWidth, App->renderer->viewportHeight); //near plane. far plane, screen width, screen height
		float4x4 view = float4x4::identity;

		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view.ptr());
	} else {
		float4x4& view = App->camera->GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view.ptr());
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj.ptr());
	glUniform4fv(glGetUniformLocation(program, "textColor"), 1, color.ptr());

	for (int i = 0; i < text.size(); ++i) {
		Character character = App->userInterface->GetCharacter(fontID, text.at(i));

		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, character.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesText[i]), &verticesText[i].front());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
}

void ComponentText::SetText(const std::string& newText) {
	text = newText;
	RecalculcateVertices();
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

//TODO make this happen with a TesseractEvent or SDLevent related to OnWindowSizeChanged
void ComponentText::RecalculcateVertices() {
	if (fontID == 0) {
		return;
	}

	verticesText.resize(text.size());

	ComponentTransform2D* transform = GetOwner().GetComponent<ComponentTransform2D>();
	float3 position = transform->GetPosition();

	float x = position.x;
	float y = position.y;

	float2 transformScale = transform->GetScale().xy();

	float scale = (fontSize / 12) * (transformScale.x > transformScale.y ? transformScale.x : transformScale.y) * GetOwner().GetComponent<ComponentCanvasRenderer>()->GetCanvasScreenFactor();

	for (int i = 0; i < text.size(); ++i) {
		Character character = App->userInterface->GetCharacter(fontID, text.at(i));

		float xpos = x + character.bearing.x * scale;
		float ypos = y - (character.size.y - character.bearing.y) * scale;

		float w = character.size.x * scale;
		float h = character.size.y * scale;

		verticesText[i] = {
			xpos,
			ypos + h,
			0.0f,
			0.0f,
			xpos,
			ypos,
			0.0f,
			1.0f,
			xpos + w,
			ypos,
			1.0f,
			1.0f,

			xpos,
			ypos + h,
			0.0f,
			0.0f,
			xpos + w,
			ypos,
			1.0f,
			1.0f,
			xpos + w,
			ypos + h,
			1.0f,
			0.0f};

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
}
