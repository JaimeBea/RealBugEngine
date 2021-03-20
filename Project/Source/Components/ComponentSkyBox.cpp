#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "ComponentSkyBox.h"

#include "Math/float4x4.cpp"
#include "GL/glew.h"

#define JSON_TAG_SKYBOX "skybox"

void ComponentSkyBox::Load(JsonValue jComponent) {
	skybox = (ResourceSkybox*) App->resources->GetResourceByID(jComponent[JSON_TAG_SKYBOX]);
}

void ComponentSkyBox::Draw() {
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	programSky = skybox->GetShader()->GetShaderProgam();
	glUseProgram(programSky);
	float4x4 proj = App->camera->GetProjectionMatrix();
	float4x4 view = App->camera->GetViewMatrix();

	glUniform1i(glGetUniformLocation(programSky, "Skybox"), 0);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "proj"), 1, GL_TRUE, &proj[0][0]);

	glBindVertexArray(skybox->GetVao());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glCubeMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	glDepthMask(GL_TRUE); // superfluous
}