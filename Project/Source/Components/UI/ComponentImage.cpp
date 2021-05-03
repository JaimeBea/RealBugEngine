#include "ComponentImage.h"

#include "GameObject.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleUserInterface.h"
#include "Panels/PanelScene.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"

#include "Utils/ImGuiUtils.h"
#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#include "Utils/Leaks.h"

#define JSON_TAG_TEXTURE_SHADERID "ShaderId"
#define JSON_TAG_TEXTURE_TEXTUREID "TextureId"
#define JSON_TAG_COLOR "Color"
#define JSON_TAG_ALPHATRANSPARENCY "AlphaTransparency"
#define JSON_TAG_ENABLED "Enabled"

ComponentImage::~ComponentImage() {
	//TODO DECREASE REFERENCE COUNT OF SHADER AND TEXTURE, MAYBE IN A NEW COMPONENT::CLEANUP?
}

void ComponentImage::Init() {
}

void ComponentImage::Update() {
}

void ComponentImage::OnEditorUpdate() {
	ImGui::TextColored(App->editor->textColor, "Texture Settings:");

	ImGui::ColorEdit4("Color##", color.ptr());

	ImGui::Checkbox("Alpha transparency", &alphaTransparency);

	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);

	UID oldID = textureID;
	ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);

	ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);

	if (textureResource != nullptr) {
		int width;
		int height;
		glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_HEIGHT, &height);

		if (oldID != textureID) {
			ComponentTransform2D* transform2D = GetOwner().GetComponent<ComponentTransform2D>();
			if (transform2D != nullptr) {
				transform2D->SetSize(float2((float) width, (float) height));
			}
		}

		ImGui::Text("");
		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Texture Preview");
		ImGui::TextWrapped("Size:");
		ImGui::SameLine();
		ImGui::TextWrapped("%d x %d", width, height);
		ImGui::Image((void*) textureResource->glTexture, ImVec2(200, 200));
		ImGui::Separator();
	}
}

void ComponentImage::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_TEXTURE_SHADERID] = shaderID;
	jComponent[JSON_TAG_TEXTURE_TEXTUREID] = textureID;
	jComponent[JSON_TAG_ENABLED] = IsActive();
	JsonValue jColor = jComponent[JSON_TAG_COLOR];
	jColor[0] = color.x;
	jColor[1] = color.y;
	jColor[2] = color.z;
	jColor[3] = color.w;

	jComponent[JSON_TAG_ALPHATRANSPARENCY] = alphaTransparency;
}

void ComponentImage::Load(JsonValue jComponent) {
	//ID == 0 means no Resource loaded
	shaderID = jComponent[JSON_TAG_TEXTURE_SHADERID];

	if (shaderID != 0) {
		App->resources->IncreaseReferenceCount(shaderID);
	}

	textureID = jComponent[JSON_TAG_TEXTURE_TEXTUREID];

	if (textureID != 0) {
		App->resources->IncreaseReferenceCount(textureID);
	}

	JsonValue jColor = jComponent[JSON_TAG_COLOR];
	color.Set(jColor[0], jColor[1], jColor[2], jColor[3]);

	alphaTransparency = jComponent[JSON_TAG_ALPHATRANSPARENCY];

	bool isEnabled = jComponent[JSON_TAG_ENABLED];
	if (isEnabled) {
		Enable();
	} else {
		Disable();
	}
}

float4 ComponentImage::GetTintColor() const {
	ComponentButton* button = GetOwner().GetComponent<ComponentButton>();
	if (button != nullptr) {
		return button->GetTintColor();
	}

	ComponentToggle* toggle = GetOwner().GetComponent<ComponentToggle>();
	if (toggle != nullptr) {
		return toggle->GetTintColor();
	}

	return float4::one;
}

void ComponentImage::Draw(const ComponentTransform2D* transform) const {
	unsigned int program = 0;
	ResourceShader* shaderResouce = App->resources->GetResource<ResourceShader>(shaderID);
	if (shaderResouce) {
		program = shaderResouce->GetShaderProgram();
	} else {
		return;
	}

	if (alphaTransparency) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glBindBuffer(GL_ARRAY_BUFFER, App->userInterface->GetQuadVBO());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
	glUseProgram(program);

	float4x4 modelMatrix;
	float4x4* proj = &App->camera->GetProjectionMatrix();

	if (App->time->IsGameRunning() || App->editor->panelScene.IsUsing2D()) {
		proj = &float4x4::D3DOrthoProjLH(-1, 1, App->renderer->GetViewportSize().x, App->renderer->GetViewportSize().y); //near plane. far plane, screen width, screen height
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

	ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);
	if (textureResource != nullptr) {
		glBindTexture(GL_TEXTURE_2D, textureResource->glTexture);
		glUniform1i(glGetUniformLocation(program, "hasDiffuse"), 1);
	} else {
		glUniform1i(glGetUniformLocation(program, "hasDiffuse"), 0);
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_BLEND);
}

void ComponentImage::DuplicateComponent(GameObject& owner) {
	ComponentImage* component = owner.CreateComponent<ComponentImage>();
	component->shaderID = shaderID;
	component->textureID = textureID;

	if (shaderID != 0) {
		App->resources->IncreaseReferenceCount(shaderID);
	}
	if (textureID != 0) {
		App->resources->IncreaseReferenceCount(textureID);
	}

	component->color = color;
	component->alphaTransparency = alphaTransparency;
}