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

void ComponentMesh::Save(JsonValue j_component) const
{
	j_component["FileName"] = mesh->file_name.c_str();
	j_component["MaterialIndex"] = mesh->material_index;
}

void ComponentMesh::Load(JsonValue j_component)
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

	DirectionalLight* directional_light = nullptr;
	std::vector<PointLight*> point_lights_vector;
	std::vector<float> point_distances_vector;
	std::vector<SpotLight*> spot_lights_vector;
	std::vector<float> spot_distances_vector;

	if (materials[mesh->material_index]->material.material_type == ShaderType::PHONG)
	{
		float far_point_distance = 0;
		PointLight* far_point_light = nullptr;
		float far_spot_distance = 0;
		SpotLight* far_spot_light = nullptr;

		for (GameObject& object : App->scene->game_objects)
		{
			ComponentDirectionalLight* dir_light = object.GetComponent<ComponentDirectionalLight>();
			if (dir_light != nullptr)
			{
				if (dir_light->IsActive())
				{
					directional_light = &dir_light->GetLightStruct();
				}
			}
			ComponentPointLight* point_light = object.GetComponent<ComponentPointLight>();
			if (point_light != nullptr)
			{
				if (point_light->IsActive())
				{
					float3 mesh_position = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float3 light_position = object.GetComponent<ComponentTransform>()->GetPosition();
					float distance = Distance(mesh_position, light_position);
					if (point_lights_vector.size() < 8)
					{
						point_distances_vector.push_back(distance);
						point_lights_vector.push_back(&point_light->GetLightStruct());

						if (distance > far_point_distance)
						{
							far_point_light = &point_light->GetLightStruct();
							far_point_distance = distance;
						}
					}
					else
					{
						if (distance < far_point_distance)
						{
							int count = 0;
							int selected = -1;
							for (float point_distance : point_distances_vector)
							{
								if (point_distance == far_point_distance) selected = count;
								count += 1;
							}

							point_lights_vector[selected] = &point_light->GetLightStruct();
							point_distances_vector[selected] = distance;

							count = 0;
							selected = -1;
							int max_distance = 0;
							for (float point_distance : point_distances_vector)
							{
								if (point_distance > max_distance)
								{
									max_distance = point_distance;
									selected = count;
								}
								count += 1;
							}

							far_point_distance = max_distance;
							far_point_light = point_lights_vector[selected];
						}
					}
				}
			}
			ComponentSpotLight* spot_light = object.GetComponent<ComponentSpotLight>();
			if (spot_light != nullptr)
			{
				if (spot_light->IsActive())
				{
					float3 mesh_position = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float3 light_position = object.GetComponent<ComponentTransform>()->GetPosition();
					float distance = Distance(mesh_position, light_position);
					if (spot_lights_vector.size() < 8)
					{
						spot_distances_vector.push_back(distance);
						spot_lights_vector.push_back(&spot_light->GetLightStruct());

						if (distance > far_spot_distance)
						{
							far_spot_light = &spot_light->GetLightStruct();
							far_spot_distance = distance;
						}
					}
					else
					{
						if (distance < far_spot_distance)
						{
							int count = 0;
							int selected = -1;
							for (float spot_distance : spot_distances_vector)
							{
								if (spot_distance == far_spot_distance) selected = count;
								count += 1;
							}

							spot_lights_vector[selected] = &spot_light->GetLightStruct();
							spot_distances_vector[selected] = distance;

							count = 0;
							selected = -1;
							int max_distance = 0;
							for (float spot_distance : spot_distances_vector)
							{
								if (spot_distance > max_distance)
								{
									max_distance = spot_distance;
									selected = count;
								}
								count += 1;
							}

							far_spot_distance = max_distance;
							far_spot_light = spot_lights_vector[selected];
						}
					}
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

		if (directional_light != nullptr)
		{
			glUniform3fv(glGetUniformLocation(program, "light.directional.direction"), 1, directional_light->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.directional.color"), 1, directional_light->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.directional.intensity"), directional_light->intensity);
		}
		glUniform1i(glGetUniformLocation(program, "light.directional.is_active"), directional_light ? 1 : 0);

		if (point_lights_vector.size() > 0)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[0].pos"), 1, point_lights_vector[0]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[0].color"), 1, point_lights_vector[0]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[0].intensity"), point_lights_vector[0]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kc"), point_lights_vector[0]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kl"), point_lights_vector[0]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kq"), point_lights_vector[0]->kq);
		}
		if (point_lights_vector.size() > 1)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[1].pos"), 1, point_lights_vector[1]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[1].color"), 1, point_lights_vector[1]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[1].intensity"), point_lights_vector[1]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kc"), point_lights_vector[1]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kl"), point_lights_vector[1]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kq"), point_lights_vector[1]->kq);
		}
		if (point_lights_vector.size() > 2)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[2].pos"), 1, point_lights_vector[2]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[2].color"), 1, point_lights_vector[2]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[2].intensity"), point_lights_vector[2]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kc"), point_lights_vector[2]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kl"), point_lights_vector[2]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kq"), point_lights_vector[2]->kq);
		}
		if (point_lights_vector.size() > 3)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[3].pos"), 1, point_lights_vector[3]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[3].color"), 1, point_lights_vector[3]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[3].intensity"), point_lights_vector[3]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kc"), point_lights_vector[3]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kl"), point_lights_vector[3]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kq"), point_lights_vector[3]->kq);
		}
		if (point_lights_vector.size() > 4)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[4].pos"), 1, point_lights_vector[4]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[4].color"), 1, point_lights_vector[4]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[4].intensity"), point_lights_vector[4]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kc"), point_lights_vector[4]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kl"), point_lights_vector[4]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kq"), point_lights_vector[4]->kq);
		}
		if (point_lights_vector.size() > 5)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[5].pos"), 1, point_lights_vector[5]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[5].color"), 1, point_lights_vector[5]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[5].intensity"), point_lights_vector[5]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kc"), point_lights_vector[5]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kl"), point_lights_vector[5]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kq"), point_lights_vector[5]->kq);
		}
		if (point_lights_vector.size() > 6)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[6].pos"), 1, point_lights_vector[6]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[6].color"), 1, point_lights_vector[6]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[6].intensity"), point_lights_vector[6]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kc"), point_lights_vector[6]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kl"), point_lights_vector[6]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kq"), point_lights_vector[6]->kq);
		}
		if (point_lights_vector.size() > 7)
		{
			glUniform3fv(glGetUniformLocation(program, "light.points[7].pos"), 1, point_lights_vector[7]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[7].color"), 1, point_lights_vector[7]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[7].intensity"), point_lights_vector[7]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kc"), point_lights_vector[7]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kl"), point_lights_vector[7]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kq"), point_lights_vector[7]->kq);
		}
		glUniform1i(glGetUniformLocation(program, "light.num_points"), point_lights_vector.size());

		if (spot_lights_vector.size() > 0)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].pos"), 1, spot_lights_vector[0]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].direction"), 1, spot_lights_vector[0]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].color"), 1, spot_lights_vector[0]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[0].intensity"), spot_lights_vector[0]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kc"), spot_lights_vector[0]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kl"), spot_lights_vector[0]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kq"), spot_lights_vector[0]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].inner_angle"), spot_lights_vector[0]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].outer_angle"), spot_lights_vector[0]->outer_angle);
		}
		if (spot_lights_vector.size() > 1)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].pos"), 1, spot_lights_vector[1]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].direction"), 1, spot_lights_vector[1]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].color"), 1, spot_lights_vector[1]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[1].intensity"), spot_lights_vector[1]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kc"), spot_lights_vector[1]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kl"), spot_lights_vector[1]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kq"), spot_lights_vector[1]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].inner_angle"), spot_lights_vector[1]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].outer_angle"), spot_lights_vector[1]->outer_angle);
		}
		if (spot_lights_vector.size() > 2)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].pos"), 1, spot_lights_vector[2]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].direction"), 1, spot_lights_vector[2]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].color"), 1, spot_lights_vector[2]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[2].intensity"), spot_lights_vector[2]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kc"), spot_lights_vector[2]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kl"), spot_lights_vector[2]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kq"), spot_lights_vector[2]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].inner_angle"), spot_lights_vector[2]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].outer_angle"), spot_lights_vector[2]->outer_angle);
		}
		if (spot_lights_vector.size() > 3)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].pos"), 1, spot_lights_vector[3]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].direction"), 1, spot_lights_vector[3]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].color"), 1, spot_lights_vector[3]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[3].intensity"), spot_lights_vector[3]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kc"), spot_lights_vector[3]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kl"), spot_lights_vector[3]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kq"), spot_lights_vector[3]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].inner_angle"), spot_lights_vector[3]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].outer_angle"), spot_lights_vector[3]->outer_angle);
		}
		if (spot_lights_vector.size() > 4)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].pos"), 1, spot_lights_vector[4]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].direction"), 1, spot_lights_vector[4]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].color"), 1, spot_lights_vector[4]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[4].intensity"), spot_lights_vector[4]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kc"), spot_lights_vector[4]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kl"), spot_lights_vector[4]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kq"), spot_lights_vector[4]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].inner_angle"), spot_lights_vector[4]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].outer_angle"), spot_lights_vector[4]->outer_angle);
		}
		if (spot_lights_vector.size() > 5)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].pos"), 1, spot_lights_vector[5]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].direction"), 1, spot_lights_vector[5]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].color"), 1, spot_lights_vector[5]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[5].intensity"), spot_lights_vector[5]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kc"), spot_lights_vector[5]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kl"), spot_lights_vector[5]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kq"), spot_lights_vector[5]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].inner_angle"), spot_lights_vector[5]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].outer_angle"), spot_lights_vector[5]->outer_angle);
		}
		if (spot_lights_vector.size() > 6)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].pos"), 1, spot_lights_vector[6]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].direction"), 1, spot_lights_vector[6]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].color"), 1, spot_lights_vector[6]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[6].intensity"), spot_lights_vector[6]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kc"), spot_lights_vector[6]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kl"), spot_lights_vector[6]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kq"), spot_lights_vector[6]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].inner_angle"), spot_lights_vector[6]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].outer_angle"), spot_lights_vector[6]->outer_angle);
		}
		if (spot_lights_vector.size() > 7)
		{
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].pos"), 1, spot_lights_vector[7]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].direction"), 1, spot_lights_vector[7]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].color"), 1, spot_lights_vector[7]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[7].intensity"), spot_lights_vector[7]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kc"), spot_lights_vector[7]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kl"), spot_lights_vector[7]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kq"), spot_lights_vector[7]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].inner_angle"), spot_lights_vector[7]->inner_angle);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].outer_angle"), spot_lights_vector[7]->outer_angle);
		}
		glUniform1i(glGetUniformLocation(program, "light.num_spots"), spot_lights_vector.size());

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
