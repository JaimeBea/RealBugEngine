#pragma once

#include "Component.h"

#include "Math/float4x4.h"

#include "GL/glew.h"
#include <vector>

class ComponentCanvasRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvasRenderer, ComponentType::CANVASRENDERER, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Render(GameObject* gameObject);
	void DuplicateComponent(GameObject& owner) override;
};