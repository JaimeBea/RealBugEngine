#include "ComponentCanvas.h"

#include "ComponentCanvasRenderer.h"

#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModulePrograms.h"

#include "Resources/GameObject.h"

void ComponentCanvas::Render(float width, float height) {
	
	float2 lastCanvasScreenSize = canvasScreenSize;

	//Change Transform2D if it w and h were changed

	canvasScreenSize.x = width;
	canvasScreenSize.y = height;

	if (lastCanvasScreenSize.x != canvasScreenSize.x) {
		//owner->transform2d.SetWidth(canvasScreenSize.x);
	}
	if (lastCanvasScreenSize.y != canvasScreenSize.y) {
		//owner->transform2d.SetHeight(canvasScreenSize.y);
	}

	//Getting the view/porj in case we need them for rendering

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	RenderGameObject(&owner);

	glDisable(GL_BLEND);

	//Canvas Scaler?

	//set width and height for transform2D?

	//Game view

	//Component canvas renderer to render Image, text, etc?

	//RenderGameObject(owner)

	
	//float4x4 viewMatrix = App->camera->GetViewMatrix();
	//float4x4 projMatrix = App->camera->GetProjectionMatrix();

	//unsigned glTexture = 0;
	//unsigned program = App->programs->defaultProgram;

	////glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, ; model from transform 2D
	//glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, viewMatrix.ptr());
	//glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, projMatrix.ptr());

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, glTexture);

	//
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//glUseProgram(0);

	//Render childs if they have canvas renderer
}

void ComponentCanvas::RenderGameObject(GameObject* gameObject) {

	ComponentCanvasRenderer* componentCanvasRenderer = gameObject->GetComponent<ComponentCanvasRenderer>();
	bool useRenderer = componentCanvasRenderer != nullptr;

	if (useRenderer) {
		componentCanvasRenderer->Render(gameObject);
	}

	for (auto& children : gameObject->GetChildren()) {
		RenderGameObject(children);
	}
}
