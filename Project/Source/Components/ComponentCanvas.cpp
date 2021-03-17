#include "ComponentCanvas.h"

#include "ComponentCanvasRenderer.h"

#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModulePrograms.h"

#include "Resources/GameObject.h"

void ComponentCanvas::Render() {
	RenderGameObject(&owner);
}

void ComponentCanvas::RenderGameObject(GameObject* gameObject) {

	ComponentCanvasRenderer* componentCanvasRenderer = gameObject->GetComponent<ComponentCanvasRenderer>();
	
	if (componentCanvasRenderer != nullptr) {
		componentCanvasRenderer->Render(gameObject);
	}

	for (auto& children : gameObject->GetChildren()) {
		RenderGameObject(children);
	}
}
