#include "ComponentCanvasRenderer.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentText.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"

#include "Utils/Leaks.h"

void ComponentCanvasRenderer::Save(JsonValue jComponent) const {
}

void ComponentCanvasRenderer::Load(JsonValue jComponent) {
}

void ComponentCanvasRenderer::Render(const GameObject* gameObject) const {
	ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();
	if (transform2D != nullptr && AnyParentHasCanvas(&GetOwner()) != nullptr) { // Get the Parent in a variable if needed and add canvas customization to render

		//IF OTHER COMPONENTS THAT RENDER IN UI ARE IMPLEMENTED, THEY MUST HAVE THEIR DRAW METHODS CALLED HERE
		ComponentImage* componentImage = gameObject->GetComponent<ComponentImage>();
		if (componentImage != nullptr) {
			componentImage->Draw(transform2D);
		}

		ComponentText* componentText = gameObject->GetComponent<ComponentText>();
		if (componentText != nullptr) {
			componentText->Draw(transform2D);
		}
	}
}

void ComponentCanvasRenderer::DuplicateComponent(GameObject& owner) {
	ComponentCanvasRenderer* component = owner.CreateComponent<ComponentCanvasRenderer>();
}

const GameObject* ComponentCanvasRenderer::AnyParentHasCanvas(const GameObject* current) const {
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
