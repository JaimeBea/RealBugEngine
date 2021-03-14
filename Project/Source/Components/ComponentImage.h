#pragma once
#include "Component.h"
#include "Math/float4x4.h"
#include <Resources/Texture.h>

class ComponentImage : public Component {
public:
	REGISTER_COMPONENT(ComponentImage, ComponentType::IMAGE);

	~ComponentImage();

	void Init() override;
	void Update() override;

	void Draw(const float4x4& modelMatrix);
	void SetTexture(Texture* text);

private:
	Texture* texture;
	unsigned int vbo;
	unsigned int renderedTexture = 0;

	void CreateVBO();
	void DestroyVBO();
};
