#pragma once

#include "Components/Component.h"
#include "Math/float2.h"
class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void Init() override;
	void DuplicateComponent(GameObject& owner) override;
	void OnEditorUpdate() override;

	void RenderGameObject(GameObject* gameObject);

	void SetScreenReferenceSize(float2 screenReferenceSize_);
	float GetScreenFactor() const;

private:
	float2 screenReferenceSize = float2(1920, 1080);
};
