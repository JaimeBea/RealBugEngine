#include "ComponentCanvas.h"
#include "ComponentCanvasRenderer.h"
#include <Modules/ModuleUserInterface.h>
#include "Application.h"
#include "GameObject.h"

void ComponentCanvas::Init() {
	App->userInterface->canvas = &GetOwner();
}

void ComponentCanvas::Save(JsonValue jComponent) const {
}

void ComponentCanvas::Load(JsonValue jComponent) {
}

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

void ComponentCanvas::DuplicateComponent(GameObject& owner) {
	ComponentCanvas* component = owner.CreateComponent<ComponentCanvas>();
	//TO DO
}
