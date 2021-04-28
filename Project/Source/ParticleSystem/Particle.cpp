#include "Particle.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentButton.h"
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
#include "Math/float3x3.h"
#include "Utils/ImGuiUtils.h"
#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#include "Utils/Leaks.h"

Particle::Particle(UID idShader, UID texture, bool alphaTrans, float vel, float4 inC, float4 finalC, float3 p) {
	shaderID = idShader;
	textureID = texture;
	alphaTransparency = alphaTrans;
	position = p;
	velocity = vel;
	currentFrame = 0;
	initColor = inC;
	isActivate = false;
	finalColor = finalC;
	localModelMatrix = float4x4::FromTRS(position, rotation, scale);
}

Particle::~Particle() {
}

void Particle::Draw(ComponentTransform* transform) {
	//unsigned int program = 0;
	//ResourceShader* shaderResouce = (ResourceShader*) App->resources->GetResource(shaderID);
	//if (shaderResouce) {
	//	program = shaderResouce->GetShaderProgram();
	//} else {
	//	return;
	//}

	//if (alphaTransparency) {
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//}

	//glBindBuffer(GL_ARRAY_BUFFER, App->userInterface->GetQuadVBO());
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
	//glUseProgram(program);
	////TODO ADD DELTATIME
	//position += float3(0, velocity, 0);
	//float4x4 ParentGlobalModelMatrix = transform->GetGlobalMatrix();
	//localModelMatrix = float4x4::FromTRS(position, rotation, float3(0.1f, 0.1, 0.1f));

	////localModelMatrix = ParentGlobalModelMatrix * localModelMatrix;

	//float3x3 rotatePart = localModelMatrix.RotatePart();
	//Frustum* frustum = App->camera->GetActiveFrustum();
	//float4x4* proj = &App->camera->GetProjectionMatrix();
	//float4x4* view = &App->camera->GetViewMatrix();

	//float4x4 newModelMatrix = localModelMatrix.LookAt(rotatePart.Col(2), -frustum->Front(), rotatePart.Col(1), float3::unitY);
	//newModelMatrix.SetTranslatePart(position);

	////-> glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, newModelMatrix.ptr());
	//glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, localModelMatrix.ptr());
	//glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view->ptr());
	//glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());

	//currentFrame += 0.01f;

	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	//glUniform1f(glGetUniformLocation(program, "currentFrame"), currentFrame);
	//glUniform4fv(glGetUniformLocation(program, "initColor"), 1, initColor.ptr());
	//glUniform4fv(glGetUniformLocation(program, "finalColor"), 1, finalColor.ptr());

	//ResourceTexture* textureResource = (ResourceTexture*) App->resources->GetResource(textureID);
	//if (textureResource != nullptr) {
	//	glBindTexture(GL_TEXTURE_2D, textureResource->glTexture);
	//}
	//glDrawArraysInstanced(GL_TRIANGLES, 0, 4, 100);
	////glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, 10);

	glDisable(GL_BLEND);
}

void Particle::Die() {
	isActivate = false;
	currentFrame = 0;
}

void Particle::Spawn(float3 p) {
	life = 5.0f;
	isActivate = true;
	position = p;
}

void Particle::Uptade() {
	life -= 0.01;
	if (life <= 0) {
		Die();
	} else {
		localModelMatrix.SetTranslatePart(position.x, position.y + 0.01, position.z);
	}
}