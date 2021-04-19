#include "ComponentSkyBox.h"

#include "Application.h"
#include "Utils/ImGuiUtils.h"
#include "Resources/ResourceShader.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleEditor.h"
#include "ComponentSkyBox.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModulePrograms.h"

#include "GL/glew.h"
#include "imgui.h"

#define JSON_TAG_SHADER "Shader"
#define JSON_TAG_SKYBOX "Skybox"

void ComponentSkyBox::Update() {
	Draw();
}

void ComponentSkyBox::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SHADER] = shaderId;
	jComponent[JSON_TAG_SKYBOX] = skyboxId;
}

void ComponentSkyBox::Load(JsonValue jComponent) {
	shaderId = jComponent[JSON_TAG_SHADER];
	if (shaderId != 0) App->resources->IncreaseReferenceCount(shaderId);
	skyboxId = jComponent[JSON_TAG_SKYBOX];
	if (skyboxId != 0) App->resources->IncreaseReferenceCount(skyboxId);
}

void ComponentSkyBox::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();

	ImGui::ResourceSlot<ResourceShader>("Shader", &shaderId);
	ImGui::ResourceSlot<ResourceSkybox>("Skybox", &skyboxId);
}

void ComponentSkyBox::Draw() {
	if (!IsActive()) return;

	ResourceShader* shader = (ResourceShader*) App->resources->GetResource(shaderId);
	if (shader == nullptr) return;
	ResourceSkybox* skybox = (ResourceSkybox*) App->resources->GetResource(skyboxId);
	if (skybox == nullptr) return;

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	unsigned int programSky = shader->GetShaderProgram();
	glUseProgram(programSky);
	float4x4 proj = App->camera->GetProjectionMatrix();
	float4x4 view = App->camera->GetViewMatrix();

	glUniformMatrix4fv(glGetUniformLocation(programSky, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "proj"), 1, GL_TRUE, &proj[0][0]);

	glBindVertexArray(skybox->GetVAO());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetGlCubeMap());
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}