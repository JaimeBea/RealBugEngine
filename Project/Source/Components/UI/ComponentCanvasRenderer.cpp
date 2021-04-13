#include "ComponentCanvasRenderer.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentCanvas.h"
#include "GameObject.h"

#include "Utils/Leaks.h"

void ComponentCanvasRenderer::Save(JsonValue jComponent) const {
}

void ComponentCanvasRenderer::Load(JsonValue jComponent) {
}

void ComponentCanvasRenderer::Render(GameObject* gameObject) {
	ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();
	ComponentCanvas* parentCanvas = AnyParentHasCanvas(&GetOwner());
	if (transform2D != nullptr && parentCanvas != nullptr) { // Get the Parent in a variable if needed and add canvas customization to render

		//IF OTHER COMPONENTS THAT RENDER IN UI ARE IMPLEMENTED, THEY MUST HAVE THEIR DRAW METHODS CALLED HERE
		ComponentImage* componentImage = gameObject->GetComponent<ComponentImage>();
		if (componentImage != nullptr) {
			componentImage->Draw(transform2D, parentCanvas);
		}

		ComponentText* componentText = gameObject->GetComponent<ComponentText>();
		if (componentText != nullptr) {
			componentText->Draw(transform2D);
		}
	}
}

float ComponentCanvasRenderer::GetCanvasScreenFactor() const {
	ComponentCanvas* parentCanvas = AnyParentHasCanvas(&GetOwner());

	return parentCanvas->GetScreenFactor();
}

void ComponentCanvasRenderer::DuplicateComponent(GameObject& owner) {
	ComponentCanvasRenderer* component = owner.CreateComponentDeferred<ComponentCanvasRenderer>();
}

ComponentCanvas* ComponentCanvasRenderer::AnyParentHasCanvas(GameObject* current) const {
	ComponentCanvas* currentCanvas = current->GetComponent<ComponentCanvas>();
	if (currentCanvas != nullptr) {
		return currentCanvas;
	} else {
		if (current->GetParent() != nullptr) {
			return AnyParentHasCanvas(current->GetParent());
		}
	}

	return nullptr;
}
