#include "ComponentImage.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Panels/PanelScene.h"
#include <Components/ComponentTransform2D.h>
#include "GameObject.h"
#include <Resources/ResourceTexture.h>
#include <Resources/ResourceShader.h>
#include "FileSystem/TextureImporter.h"
#include "Math/TransformOps.h"
#include "FileSystem/JsonValue.h"
#include "Utils/ImGuiUtils.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#define JSON_TAG_TEXTURE_SHADERID "ShaderID"
#define JSON_TAG_TEXTURE_TEXTUREID "TextureID"
#define JSON_TAG_COLOR "Color"
#define JSON_TAG_ALPHATRANSPARENCY "AlphaTransparency"

ComponentImage::~ComponentImage() {
	DestroyVBO();
}

void ComponentImage::Init() {
	CreateVBO();
}

void ComponentImage::Update() {
}

void ComponentImage::OnEditorUpdate() {
	ImGui::TextColored(App->editor->textColor, "Texture Settings:");

	ImGui::ColorEdit4("Color##", color.ptr());

	ImGui::Checkbox("Alpha transparency", &alphaTransparency);

	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);
	ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);
	ResourceTexture* tex = (ResourceTexture*) App->resources->GetResource(textureID);

	if (tex != nullptr) {
		ImGui::Text("");
		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Texture Preview");
		ImGui::TextWrapped("Size:");
		ImGui::SameLine();
		int width;
		int height;
		glGetTextureLevelParameteriv(tex->glTexture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(tex->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
		ImGui::TextWrapped("%d x %d", width, height);
		ImGui::Image((void*) tex->glTexture, ImVec2(200, 200));
		ImGui::Separator();
	}
}

void ComponentImage::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_TEXTURE_SHADERID] = shaderID;
	jComponent[JSON_TAG_TEXTURE_TEXTUREID] = textureID;

	JsonValue jColor = jComponent[JSON_TAG_COLOR];
	jColor[0] = color.x;
	jColor[1] = color.y;
	jColor[2] = color.z;
	jColor[3] = color.w;

	jComponent[JSON_TAG_ALPHATRANSPARENCY] = alphaTransparency;
}

void ComponentImage::Load(JsonValue jComponent) {
	shaderID = jComponent[JSON_TAG_TEXTURE_SHADERID];
	App->resources->IncreaseReferenceCount(shaderID);

	textureID = jComponent[JSON_TAG_TEXTURE_TEXTUREID];
	App->resources->IncreaseReferenceCount(textureID);

	JsonValue jColor = jComponent[JSON_TAG_COLOR];
	color.Set(jColor[0], jColor[1], jColor[2], jColor[3]);

	alphaTransparency = jComponent[JSON_TAG_ALPHATRANSPARENCY];
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

void ComponentImage::DestroyVBO() {
	glDeleteBuffers(1, &vbo);
}

const float4 ComponentImage::GetTintColor() const {
	ComponentButton* button = GetOwner().GetComponent<ComponentButton>();
	if (button != nullptr) {
		return button->GetTintColor();
	}
	return float4::one;
}

void ComponentImage::Draw(ComponentTransform2D* transform) {
	unsigned int program = 0;
	ResourceShader* shaderResouce = (ResourceShader*) App->resources->GetResource(shaderID);
	if (shaderResouce) {
		program = shaderResouce->GetShaderProgram();
	}
	ResourceTexture* texResource = (ResourceTexture*) App->resources->GetResource(textureID);
	if (texResource == nullptr) return;

	if (alphaTransparency) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
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

	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	glUniform4fv(glGetUniformLocation(program, "inputColor"), 1, color.ptr());
	glUniform4fv(glGetUniformLocation(program, "tintColor"), 1, GetTintColor().ptr());

	glBindTexture(GL_TEXTURE_2D, texResource->glTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_BLEND);
}

void ComponentImage::DuplicateComponent(GameObject& owner) {
	ComponentImage* component = owner.CreateComponent<ComponentImage>();
	//TO DO
}