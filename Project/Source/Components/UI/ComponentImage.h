#pragma once
#include <Components/Component.h>
#include "Resources/ResourceTexture.h"
#include "Math/float4.h"

class ComponentTransform2D;

class ComponentImage : public Component {
public:
	REGISTER_COMPONENT(ComponentImage, ComponentType::IMAGE, false);

	~ComponentImage();

	void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void DuplicateComponent(GameObject& owner) override;
	void Draw(ComponentTransform2D* transform);
	void SetTextureID(UID uid);

private:
	float4 color = {1, 1, 1, 1};
	unsigned int vbo;
	bool alphaTransparency = false;
	UID textureID;
	UID shaderID;

	void CreateVBO();
	void DestroyVBO();

	const float4 GetTintColor() const;
};
