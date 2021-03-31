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
	if (transform2D != nullptr && AnyParentHasCanvas(&GetOwner()) != nullptr) {		// Get the Parent in a variable if needed and add canvas customization to render
		ComponentImage* componentImage = gameObject->GetComponent<ComponentImage>();
		if (componentImage != nullptr) {
			componentImage->Draw(transform2D);
		}
	}
}

void ComponentCanvasRenderer::DuplicateComponent(GameObject& owner) {
	ComponentCanvasRenderer* component = owner.CreateComponent<ComponentCanvasRenderer>();
	//TO DO
}

GameObject* ComponentCanvasRenderer::AnyParentHasCanvas(GameObject* current) {
	ComponentCanvas* currentCanvas = current->GetComponent<ComponentCanvas>();
	if (currentCanvas != nullptr) {
		return current;
	} else {
		if (current->GetParent() != nullptr) {
			return AnyParentHasCanvas(current->GetParent());
		}
	}

	return nullptr;
}
