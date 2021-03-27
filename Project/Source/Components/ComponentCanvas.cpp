#include "ComponentCanvas.h"
#include "ComponentCanvasRenderer.h"
#include "Application.h"
#include "Resources/GameObject.h"

void ComponentCanvas::Render() {
	RenderGameObject(&GetOwner());
}

void ComponentCanvas::RenderGameObject(GameObject* gameObject) {

	ComponentCanvasRenderer* componentCanvasRenderer = gameObject->GetComponent<ComponentCanvasRenderer>();
	
	if (componentCanvasRenderer != nullptr) {
		componentCanvasRenderer->Render(gameObject);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		RenderGameObject(child);
	}
}
