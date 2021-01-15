#pragma once

#include "Component.h"
#include "Material.h"

class ComponentMaterial : public Component
{
public:
	REGISTER_COMPONENT(ComponentMaterial, ComponentType::MATERIAL);

	void OnEditorUpdate() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

public:
	GameObject* light = nullptr;

	Material material;
};
