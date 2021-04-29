#pragma once

#include "Components/Component.h"

class ComponentCanvas;

class ComponentCanvasRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvasRenderer, ComponentType::CANVASRENDERER, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;
	bool CanBeRemoved() const override;
	void Render(const GameObject* gameObject) const; //Calls Draw on all images if one of its parents is a canvas
	float GetCanvasScreenFactor() const;

private:
	const ComponentCanvas* AnyParentHasCanvas(GameObject* current) const; //Recursively iterates through parents to check if one of them contains a Canvas Component
};
