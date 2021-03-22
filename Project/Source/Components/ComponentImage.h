#pragma once
#include "Component.h"
#include <Resources/Texture.h>
#include "ComponentTransform2D.h"

class ComponentImage : public Component {
public:
	REGISTER_COMPONENT(ComponentImage, ComponentType::IMAGE);

	~ComponentImage();

	void Init() override;
	void Update() override;

	void Draw(ComponentTransform2D* transform);
	void SetTexture(Texture* text);

private:
	Texture* texture;
	unsigned int vbo;
	unsigned int renderedTexture = 0;

	void CreateVBO();
	void DestroyVBO();
};
