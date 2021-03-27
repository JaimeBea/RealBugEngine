#pragma once

#include "Component.h"

#include "Math/float4x4.h"
#include "Math/float2.h"

#include "GL/glew.h"
#include <vector>

class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS, false);

	void Render();
	void RenderGameObject(GameObject* gameObject);
	void DuplicateComponent(GameObject& owner) override;
};
