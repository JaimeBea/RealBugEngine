#pragma once

#include <Components/Component.h>
#include "Resources/ResourceTexture.h"
#include "UI/FontImporter.h"
#include <Components/ComponentTransform2D.h>
#include <Math/float3.h>

class ComponentText : public Component {
public:
	REGISTER_COMPONENT(ComponentText, ComponentType::TEXT, false);

	~ComponentText();

	void Init() override;
	void Update() override;
	void OnEditorUpdate() override;

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Draw(ComponentTransform2D* transform);
	void SetText(const std::string& newText);

	void SetFontSize(float newfontSize);

	void SetFontColor(const float4& newColor);
	float4 GetFontColor() const;

private:
	std::string text = "Text";

	float fontSize = 12.0f;
	float4 color = float4::one;

	unsigned int vbo;
	unsigned int vao;
	UID shaderID = 0;
	UID fontID = 0;
};