#pragma once

#include "Component.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <unordered_map>

struct aiMesh;

class ComponentMeshRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentMeshRenderer, ComponentType::MESH);

	void OnEditorUpdate() override;
	void Init() override;
	void Update() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Draw(const float4x4& modelMatrix);

public:
	Mesh* mesh = nullptr;
	Material material;
	std::vector<float4x4> palette;

	std::unordered_map<std::string, GameObject*> goBones;
};
