#pragma once

#include "Components/Component.h"
#include "Math/float2.h"
class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Init() override;
	void RenderGameObject(GameObject* gameObject);
	void DuplicateComponent(GameObject& owner) override;
	float GetScreenFactor() const;

private:
	float2 screenBaseRes = float2(1920, 1080);
};
