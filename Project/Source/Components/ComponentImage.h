#pragma once
#include "Component.h"
#include <Resources/ResourceTexture.h>
#include "ComponentTransform2D.h"

class ComponentImage : public Component {
public:
	REGISTER_COMPONENT(ComponentImage, ComponentType::IMAGE, false);

	~ComponentImage();

	void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	void DuplicateComponent(GameObject& owner) override;

	void Draw(ComponentTransform2D* transform);
	void SetTexture(ResourceTexture* text);

private:
	ResourceTexture* texture;
	float3 color = {255, 255, 255};
	unsigned int vbo;
	unsigned int renderedTexture = 0;

	void CreateVBO();
	void DestroyVBO();
};
