#pragma once

#include "Component.h"
#include "Mesh.h"

#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <vector>

class ComponentMaterial;
struct aiMesh;

class ComponentMesh : public Component
{
public:
	REGISTER_COMPONENT(ComponentMesh, ComponentType::MESH);

	void OnEditorUpdate() override;

	void Draw(const std::vector<ComponentMaterial*>& materials, const float4x4& model_matrix) const;

public:
	Mesh* mesh = nullptr;
};
