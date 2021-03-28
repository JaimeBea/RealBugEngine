#include "ComponentCanvasRenderer.h"
#include "UI/ComponentImage.h"
#include "ComponentTransform2D.h"

#include "GameObject.h"

void ComponentCanvasRenderer::Save(JsonValue jComponent) const {
}

void ComponentCanvasRenderer::Load(JsonValue jComponent) {
}

void ComponentCanvasRenderer::Render(GameObject* gameObject) {
	ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();

	ComponentImage* componentImage = gameObject->GetComponent<ComponentImage>();
	if (componentImage != nullptr) {
		componentImage->Draw(transform2D);
	}
}

void ComponentCanvasRenderer::DuplicateComponent(GameObject& owner) {
	ComponentCanvasRenderer* component = owner.CreateComponent<ComponentCanvasRenderer>();
	//TO DO
}
