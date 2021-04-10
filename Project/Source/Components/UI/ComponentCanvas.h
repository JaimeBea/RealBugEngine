#pragma once

#include "Components/Component.h"

#include "Math/float4x4.h"
#include "Math/float2.h"

#include "GL/glew.h"
#include <vector>

class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Init() override;
	void RenderGameObject(GameObject* gameObject);
	void DuplicateComponent(GameObject& owner) override;
};
