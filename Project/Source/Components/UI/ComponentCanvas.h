#pragma once

#include "Components/Component.h"

class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Init() override;
	void RenderGameObject(GameObject* gameObject);
	void DuplicateComponent(GameObject& owner) override;
};
