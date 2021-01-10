#include "ComponentMesh.h"

#include "Globals.h"
#include "Logging.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentLight.h"
#include "ComponentBoundingBox.h"
#include "Application.h"
#include "ModulePrograms.h"
#include "ModuleResources.h"
#include "ModuleCamera.h"
#include "ModuleSceneRender.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "Texture.h"
#include "Mesh.h"
#include "MeshImporter.h"

#include "assimp/mesh.h"
#include "GL/glew.h"
#include "imgui.h"

#include "Leaks.h"

void ComponentMesh::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector<ComponentMesh*> meshes = selected->GetComponents<ComponentMesh>();
	int count = 1;

	for (ComponentMesh* mesh : meshes)
	{
		// Show only # when multiple
		char name[50];
		if (meshes.size() == 1)
		{
			sprintf_s(name, 50, "Mesh");
		}
		else
		{
			sprintf_s(name, 50, "Mesh %d", count);
		}

		if (ImGui::CollapsingHeader(name))
		{
			bool active = IsActive();
			if (ImGui::Checkbox("Active##mesh", &active))
			{
				active ? Enable() : Disable();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##mesh"))
			{
				// TODO: Add delete Component tool
			}
			ImGui::Separator();

			ImGui::TextColored(title_color, "Geometry");
			ImGui::TextWrapped("Num Vertices: ");
			ImGui::SameLine();
			ImGui::TextColored(text_color, "%d", mesh->mesh->num_vertices);
			ImGui::TextWrapped("Num Triangles: ");
			ImGui::SameLine();
			ImGui::TextColored(text_color, "%d", mesh->mesh->num_indices / 3);
			ImGui::Separator();
			ImGui::TextColored(title_color, "Bounding Box");
			ImGui::Checkbox("Draw##mesh", &bb_active);
			if (bb_active)
			{
				ComponentBoundingBox* bounding_box = selected->GetComponent<ComponentBoundingBox>();
				bounding_box->DrawBoundingBox();
			}
			ImGui::Separator();
		}
		count++;
	}
}

void ComponentMesh::Save(JsonValue& j_component) const
{
	j_component["FileName"] = mesh->file_name.c_str();
	j_component["MaterialIndex"] = mesh->material_index;
}

void ComponentMesh::Load(const JsonValue& j_component)
{
	if (mesh == nullptr) mesh = App->resources->ObtainMesh();

	MeshImporter::UnloadMesh(mesh);
	mesh->file_name = j_component["FileName"];
	mesh->material_index = j_component["MaterialIndex"];
	MeshImporter::LoadMesh(mesh);
}

void ComponentMesh::Draw(const std::vector<ComponentMaterial*>& materials, const float4x4& model_matrix) const
{
	if (!IsActive()) return;

	unsigned program = App->programs->default_program;

	float4x4 view_matrix = App->camera->GetViewMatrix();
	float4x4 proj_matrix = App->camera->GetProjectionMatrix();
	unsigned gl_texture = 0;
	ComponentLight* light;
	if (materials.size() > mesh->material_index)
	{
		if (materials[mesh->material_index]->IsActive())
		{
			Texture* diffuse = materials[mesh->material_index]->diffuse_map;
			gl_texture = diffuse ? diffuse->gl_texture : 0;
		}
	}

	if (materials[mesh->material_index]->material_type == ShaderType::PHONG)
	{
		float3 light_position = float3(0, 0, 0);
		float3 light_color = float3(0, 0, 0);

		// TODO: Improve after Light class
		for (GameObject* object : App->scene->root->GetChildren())
		{
			light = object->GetComponent<ComponentLight>();
			if (light != nullptr)
			{
				if (light->IsActive())
				{
					light_color = light->light.color;
				}
				light_position = object->GetComponent<ComponentTransform>()->GetPosition();
				break;
			}
		}
		program = App->programs->phong_pbr_program;
		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "material.diffuse_color"), 1, materials[mesh->material_index]->diffuse_color.ptr());
		glUniform3fv(glGetUniformLocation(program, "material.specular_color"), 1, materials[mesh->material_index]->specular_color.ptr());
		glUniform1f(glGetUniformLocation(program, "material.shininess"), materials[mesh->material_index]->shininess);
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, App->scene_renderer->ambient_color.ptr());

		int has_diffuse_map = (materials[mesh->material_index]->has_diffuse_map) ? 1 : 0;
		int has_specular_map = (materials[mesh->material_index]->has_specular_map) ? 1 : 0;
		int shininess_alpha = (materials[mesh->material_index]->has_shininess_in_alpha_channel) ? 1 : 0;
		glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), has_diffuse_map);
		glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), has_specular_map);
		glUniform1i(glGetUniformLocation(program, "material.shininess_alpha"), shininess_alpha);

		glUniform3fv(glGetUniformLocation(program, "light.position"), 1, light_position.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.color"), 1, light_color.ptr());
		glUniform3fv(glGetUniformLocation(program, "view_pos"), 1, App->camera->GetPosition().ptr());
	}
	else
	{
		glUseProgram(program);
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_matrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj_matrix.ptr());
	glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), 0);

	// TODO: Display Specular Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_texture);

	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
