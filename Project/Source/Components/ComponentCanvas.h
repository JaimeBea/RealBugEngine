#pragma once

#include "Component.h"

#include "Math/float4x4.h"
#include "Math/float2.h"

#include "GL/glew.h"
#include <vector>

class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS);

	void Render(float width, float height);
	void RenderGameObject(GameObject* gameObject);

private:
	float2 canvasScreenSize = float2::zero;
	float2 canvasScreenPosition = float2::zero;
};
