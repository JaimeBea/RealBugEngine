#pragma once

#include "Component.h"

#include "Math/float4x4.h"

#include "GL/glew.h"
#include <vector>


class ComponentCanvasRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvasRenderer, ComponentType::CANVASRENDERER);

	void Render(GameObject* gameObject);

};