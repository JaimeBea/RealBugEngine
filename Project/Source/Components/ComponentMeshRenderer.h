#pragma once

#include "Component.h"
#include "Resources/Mesh.h"

#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <vector>

class ComponentMaterial;
struct aiMesh;

class ComponentMeshRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentMeshRenderer, ComponentType::MESH);

	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Draw(const std::vector<ComponentMaterial*>& materials, const float4x4& modelMatrix) const;

public:
	Mesh* mesh = nullptr;

private:
	bool bbActive = false;
};
