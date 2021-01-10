#pragma once

#include "Component.h"

#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <vector>

class ComponentMaterial;
struct aiMesh;

class ComponentMesh : public Component
{
public:
	REGISTER_COMPONENT(ComponentType::MESH);

	ComponentMesh(GameObject& owner);

	void OnEditorUpdate() override;

	void Load(const aiMesh* mesh);
	void Release();
	void Draw(const std::vector<ComponentMaterial*>& materials, const float4x4& model_matrix) const;

public:
	unsigned vbo = 0;
	unsigned ebo = 0;
	unsigned vao = 0;
	unsigned num_vertices = 0;
	unsigned num_indices = 0;
	unsigned material_index = 0;

private:
	bool bb_active = false;
};
