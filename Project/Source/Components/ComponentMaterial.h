#pragma once

#include "Component.h"
#include "Resources/Material.h"

class ComponentMaterial : public Component {
public:
	REGISTER_COMPONENT(ComponentMaterial, ComponentType::MATERIAL);

	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

public:
	GameObject* light = nullptr;

	Material material;
};
