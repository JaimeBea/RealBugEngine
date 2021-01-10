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
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

	void Draw(const std::vector<ComponentMaterial*>& materials, const float4x4& model_matrix) const;

public:
	Mesh* mesh = nullptr;

private:
	bool bb_active = false;
};
