#include "ComponentCanvasRenderer.h"
#include "ComponentImage.h"
#include "ComponentTransform2D.h"

#include "GameObject.h"

void ComponentCanvasRenderer::Render(GameObject* gameObject) {
	//We should get UI components (image, text,etc) here and call their render functions

	ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();

	ComponentImage* componentImage = gameObject->GetComponent<ComponentImage>();
	if (componentImage != nullptr) {
		componentImage->Draw(transform2D);
	}

	/*ComponentText* componentText = gameObject->GetComponent<componentText>();
	if (componentText != nullptr) {
		componentText->Render();
	}*/
}

void ComponentCanvasRenderer::DuplicateComponent(GameObject& owner) {
	ComponentCanvasRenderer* component = owner.CreateComponent<ComponentCanvasRenderer>();
	//TO DO
}
