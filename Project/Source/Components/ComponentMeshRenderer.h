#pragma once

#include "Component.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <vector>

struct aiMesh;

class ComponentMeshRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentMeshRenderer, ComponentType::MESH);

	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Draw(const float4x4& modelMatrix) const;

public:
	Mesh* mesh = nullptr;
	Material material;

private:
	bool bbActive = false;
};