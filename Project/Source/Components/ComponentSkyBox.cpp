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

#define JSON_TAG_SKYBOX "skybox"

void ComponentSkyBox::OnTransformUpdate() {
}

void ComponentSkyBox::Init() {
	OnTransformUpdate();
}

void ComponentSkyBox::Update() {
	Draw();
}

void ComponentSkyBox::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SKYBOX] = skyboxId;
}

void ComponentSkyBox::Load(JsonValue jComponent) {
	skyboxId = jComponent[JSON_TAG_SKYBOX];
	if (skyboxId != 0) App->resources->IncreaseReferenceCount(skyboxId);
}

void ComponentSkyBox::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();

	ImGui::ResourceSlot<ResourceSkybox>("Skybox", &skyboxId);

	if (ImGui::TreeNode("Skybox")) {
		ResourceSkybox* skybox = (ResourceSkybox*) App->resources->GetResource(skyboxId);
		if (skybox != nullptr) {
			ImGui::ResourceSlot<ResourceShader>("Shader", &skybox->shaderId);
			ResourceShader* shader = (ResourceShader*) App->resources->GetResource(skybox->shaderId);
		}
		ImGui::TreePop();
	}
}

void ComponentSkyBox::Draw() {
	ResourceSkybox* skybox = (ResourceSkybox*) App->resources->GetResource(skyboxId);
	if (skybox == nullptr) return;
	ResourceShader* shader = (ResourceShader*) App->resources->GetResource(skybox->shaderId);
	if (shader == nullptr) return;

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	programSky = shader->GetShaderProgram();
	glUseProgram(programSky);
	float4x4 proj = App->camera->GetProjectionMatrix();
	float4x4 view = App->camera->GetViewMatrix();

	glUniform1i(glGetUniformLocation(programSky, "Skybox"), 0);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "proj"), 1, GL_TRUE, &proj[0][0]);

	glBindVertexArray(skybox->GetVao());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetGlCubeMap());
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}