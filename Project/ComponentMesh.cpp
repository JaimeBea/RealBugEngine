#include "ComponentMesh.h"

#include "Globals.h"
#include "Logging.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentDirectionalLight.h"
#include "ComponentPointLight.h"
#include "ComponentSpotLight.h"
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
			sprintf_s(name, 50, "Mesh %d##mesh_%d", count, count);
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
	unsigned gl_texture_diffuse = 0;
	unsigned gl_texture_specular = 0;

	DirectionalLight* dir_light_struct = nullptr;
	PointLight* point_light_struct = nullptr;
	SpotLight* spot_light_struct = nullptr;

	if (materials.size() > mesh->material_index)
	{
		if (materials[mesh->material_index]->IsActive())
		{
			Texture* diffuse = materials[mesh->material_index]->material.diffuse_map;
			gl_texture_diffuse = diffuse ? diffuse->gl_texture : 0;
			Texture* specular = materials[mesh->material_index]->material.specular_map;
			gl_texture_specular = specular ? specular->gl_texture : 0;
		}
	}

	if (materials[mesh->material_index]->material.material_type == ShaderType::PHONG)
	{
		for (GameObject* object : App->scene->root->GetChildren())
		{
			ComponentDirectionalLight* dir_light = object->GetComponent<ComponentDirectionalLight>();
			if (dir_light != nullptr)
			{
				if (dir_light->IsActive())
				{
					dir_light_struct = &dir_light->GetLightStruct();
				}
			}
			ComponentPointLight* point_light = object->GetComponent<ComponentPointLight>();
			if (point_light != nullptr)
			{
				if (point_light->IsActive())
				{
					point_light_struct = &point_light->GetLightStruct();
				}
			}
			ComponentSpotLight* spot_light = object->GetComponent<ComponentSpotLight>();
			if (spot_light != nullptr)
			{
				if (spot_light->IsActive())
				{
					spot_light_struct = &spot_light->GetLightStruct();
				}
			}
		}
		program = App->programs->phong_pbr_program;
		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "diffuse_color"), 1, materials[mesh->material_index]->material.diffuse_color.ptr());
		glUniform3fv(glGetUniformLocation(program, "specular_color"), 1, materials[mesh->material_index]->material.specular_color.ptr());
		glUniform1f(glGetUniformLocation(program, "shininess"), materials[mesh->material_index]->material.shininess);

		int has_diffuse_map = (materials[mesh->material_index]->material.has_diffuse_map) ? 1 : 0;
		int has_specular_map = (materials[mesh->material_index]->material.has_specular_map) ? 1 : 0;
		int shininess_alpha = (materials[mesh->material_index]->material.has_shininess_in_alpha_channel) ? 1 : 0;
		glUniform1i(glGetUniformLocation(program, "has_diffuse_map"), has_diffuse_map);
		glUniform1i(glGetUniformLocation(program, "has_specular_map"), has_specular_map);
		glUniform1i(glGetUniformLocation(program, "shininess_alpha"), shininess_alpha);

		glUniform3fv(glGetUniformLocation(program, "light.ambient.color"), 1, App->scene_renderer->ambient_color.ptr());

		glUniform3fv(glGetUniformLocation(program, "light.directional.direction"), 1, dir_light_struct->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.directional.color"), 1, dir_light_struct->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.directional.intensity"), dir_light_struct->intensity);

		glUniform3fv(glGetUniformLocation(program, "light.points[i].pos"), 1, point_light_struct->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points.color"), 1, point_light_struct->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points.intensity"), point_light_struct->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points.kc"), point_light_struct->kc);
		glUniform1f(glGetUniformLocation(program, "light.points.kl"), point_light_struct->kl);
		glUniform1f(glGetUniformLocation(program, "light.points.kq"), point_light_struct->kq);

		glUniform3fv(glGetUniformLocation(program, "light.spot.pos"), 1, spot_light_struct->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spot.direction"), 1, spot_light_struct->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spot.color"), 1, spot_light_struct->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spot.intensity"), spot_light_struct->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spot.kc"), spot_light_struct->kc);
		glUniform1f(glGetUniformLocation(program, "light.spot.kl"), spot_light_struct->kl);
		glUniform1f(glGetUniformLocation(program, "light.spot.kq"), spot_light_struct->kq);
		glUniform1f(glGetUniformLocation(program, "light.spot.inner_angle"), spot_light_struct->inner_angle);
		glUniform1f(glGetUniformLocation(program, "light.spot.outer_angle"), spot_light_struct->outer_angle);

		glUniform3fv(glGetUniformLocation(program, "view_pos"), 1, App->camera->GetPosition().ptr());
	}
	else
	{
		glUseProgram(program);
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_matrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj_matrix.ptr());
	glUniform1i(glGetUniformLocation(program, "diffuse_map"), 0);
	glUniform1i(glGetUniformLocation(program, "specular_map"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_texture_diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gl_texture_specular);

	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
