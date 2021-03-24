#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleEditor.h"
#include "ComponentSkyBox.h"

#include "GL/glew.h"
#include "imgui.h"

#define JSON_TAG_SKYBOX "skybox"

void ComponentSkyBox::OnTransformUpdate() {
}

void ComponentSkyBox::Init() {
	OnTransformUpdate();
}

void ComponentSkyBox::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SKYBOX] = skybox->GetId();
}

void ComponentSkyBox::Load(JsonValue jComponent) {
	skybox = (ResourceSkybox*) App->resources->GetResourceByID(jComponent[JSON_TAG_SKYBOX]);
}

void ComponentSkyBox::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	// MESH
	if (ImGui::TreeNode("Mesh")) {
		ImGui::TextColored(App->editor->titleColor, "Geometry");
		ImGui::TextWrapped("Num Vertices: ");
		ImGui::SameLine();
		//ImGui::TextColored(App->editor->textColor, "%d", mesh->numVertices);
		//ImGui::TextWrapped("Num Triangles: ");
		ImGui::SameLine();
		/*ImGui::TextColored(App->editor->textColor, "%d", mesh->numIndices / 3);*/
		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Bounding Box");

		//ImGui::Checkbox("Draw", &bbActive);
		/*if (bbActive) {
			ComponentBoundingBox* boundingBox = GetOwner().GetComponent<ComponentBoundingBox>();
			boundingBox->DrawBoundingBox();
		}*/
		ImGui::Separator();
	}
}

void ComponentSkyBox::Draw() {
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	/*programSky = skybox->GetShader()->GetShaderProgram();*/
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
	glDepthFunc(GL_LESS); // set depth function back to default
	glDepthMask(GL_TRUE); // superfluous
}