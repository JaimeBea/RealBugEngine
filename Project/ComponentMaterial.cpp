#include "ComponentMaterial.h"

#include "Application.h"
#include "TextureImporter.h"
#include "GameObject.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui.h"
#include "GL/glew.h"

#include "Leaks.h"

void ComponentMaterial::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector<ComponentMaterial*> materials = selected->GetComponents<ComponentMaterial>();
	int count = 1;

	for (ComponentMaterial* material : materials)
	{
		// Show only # when multiple
		char name[50];
		if (materials.size() == 1)
		{
			sprintf_s(name, 50, "Material");
		}
		else
		{
			sprintf_s(name, 50, "Material %d##material_%d", count, count);
		}

		if (ImGui::CollapsingHeader(name))
		{
			bool active = IsActive();
			sprintf_s(name, 50, "Active##material_%d", count);
			if (ImGui::Checkbox(name, &active))
			{
				active ? Enable() : Disable();
			}
			ImGui::SameLine();
			sprintf_s(name, 50, "Remove##material_%d", count);
			if (ImGui::Button(name))
			{
				// TODO: Fix me
				// selected->RemoveComponent(material);
				// continue;
			}
			ImGui::Separator();

			ImGui::TextColored(App->editor->title_color, "Shader");

			// Material types
			const char* material_types[] = {"Standard", "Phong"};
			const char* material_types_current = material_types[(int) material->material.material_type];
			sprintf_s(name, 50, "Type##material_%d", count);
			if (ImGui::BeginCombo(name, material_types_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(material_types); ++n)
				{
					bool is_selected = (material_types_current == material_types[n]);
					if (ImGui::Selectable(material_types[n], is_selected))
					{
						material->material.material_type = ShaderType(n);
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
				ImGui::Text("");
			}
			if (material->material.material_type == ShaderType::PHONG)
			{
				std::vector<Texture*> textures;
				for (Texture& texture : App->resources->textures) textures.push_back(&texture);

				// Diffuse Texture Combo
				const char* diffuse_items[] = {"Diffuse Color", "Diffuse Texture"};
				const char* diffuse_item_current = diffuse_items[material->material.has_diffuse_map];
				ImGui::TextColored(App->editor->text_color, "Diffuse Settings:");
				sprintf_s(name, 50, "##diffuse_material_%d", count);
				if (ImGui::BeginCombo(name, diffuse_item_current))
				{
					for (int n = 0; n < IM_ARRAYSIZE(diffuse_items); ++n)
					{
						bool is_selected = (diffuse_item_current == diffuse_items[n]);
						if (ImGui::Selectable(diffuse_items[n], is_selected))
						{
							material->material.has_diffuse_map = n ? 1 : 0;
						}
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (diffuse_item_current == diffuse_items[0])
				{
					sprintf_s(name, 50, "Color##diffuse_color_%d", count);
					ImGui::ColorEdit3(name, material->material.diffuse_color.ptr());
				}
				else
				{
					std::string& current_diffuse_texture = material->material.diffuse_map ? material->material.diffuse_map->file_name : "";
					sprintf_s(name, 50, "Texture##diffuse_texture_%d", count);
					if (ImGui::BeginCombo(name, current_diffuse_texture.c_str()))
					{
						for (unsigned i = 0; i < textures.size(); ++i)
						{
							bool is_selected = (current_diffuse_texture == textures[i]->file_name);
							if (ImGui::Selectable(textures[i]->file_name.c_str(), is_selected))
							{
								material->material.diffuse_map = textures[i];
							};
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}
				ImGui::Text("");

				// Specular Texture Combo
				const char* specular_items[] = {"Specular Color", "Specular Texture"};
				const char* specular_item_current = specular_items[material->material.has_specular_map];
				ImGui::TextColored(App->editor->text_color, "Specular Settings:");
				sprintf_s(name, 50, "##specular_%d", count);
				if (ImGui::BeginCombo(name, specular_item_current))
				{
					for (int n = 0; n < IM_ARRAYSIZE(specular_items); ++n)
					{
						bool is_selected = (specular_item_current == specular_items[n]);
						if (ImGui::Selectable(specular_items[n], is_selected))
						{
							material->material.has_specular_map = n ? 1 : 0;
						};
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (specular_item_current == specular_items[0])
				{
					sprintf_s(name, 50, "Color##specular_color_%d", count);
					ImGui::ColorEdit3(name, material->material.specular_color.ptr());
				}
				else
				{
					std::string& current_specular_texture = material->material.specular_map ? material->material.specular_map->file_name : "";
					sprintf_s(name, 50, "Texture##specular_texture_%d", count);
					if (ImGui::BeginCombo(name, current_specular_texture.c_str()))
					{
						for (unsigned i = 0; i < textures.size(); ++i)
						{
							bool is_selected = (current_specular_texture == textures[i]->file_name);
							if (ImGui::Selectable(textures[i]->file_name.c_str(), is_selected))
							{
								material->material.specular_map = textures[i];
							};
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}

				// Shininess Combo
				const char* shininess_items[] = {"Shininess Value", "Shininess Alpha"};
				const char* shininess_item_current = shininess_items[material->material.has_shininess_in_alpha_channel];
				sprintf_s(name, 50, "##shininess_%d", count);
				if (ImGui::BeginCombo(name, shininess_item_current))
				{
					for (int n = 0; n < IM_ARRAYSIZE(shininess_items); ++n)
					{
						bool is_selected = (shininess_item_current == shininess_items[n]);
						if (ImGui::Selectable(shininess_items[n], is_selected))
						{
							material->material.has_shininess_in_alpha_channel = n ? 1 : 0;
						}
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (shininess_item_current == shininess_items[0])
				{
					sprintf_s(name, 50, "Shininess##shininess_value_%d", count);
					ImGui::DragFloat(name, &material->material.shininess, App->editor->drag_speed3f, 0.0f, 1000.0f);
				}
			}
			ImGui::Separator();
			ImGui::TextColored(App->editor->title_color, "Filters");

			// Min filter combo box
			const char* min_filter_items[] = {"Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear"};
			const char* min_filter_item_current = min_filter_items[int(App->resources->GetMinFilter())];
			sprintf_s(name, 50, "Min filter##min_filter_%d", count);
			if (ImGui::BeginCombo(name, min_filter_item_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(min_filter_items); ++n)
				{
					bool is_selected = (min_filter_item_current == min_filter_items[n]);
					if (ImGui::Selectable(min_filter_items[n], is_selected))
					{
						App->resources->SetMinFilter(TextureMinFilter(n));
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// Mag filter combo box
			const char* mag_filter_items[] = {"Nearest", "Linear"};
			const char* mag_filter_item_current = mag_filter_items[int(App->resources->GetMagFilter())];
			sprintf_s(name, 50, "Mag filter##mag_filter_%d", count);
			if (ImGui::BeginCombo(name, mag_filter_item_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(mag_filter_items); ++n)
				{
					bool is_selected = (mag_filter_item_current == mag_filter_items[n]);
					if (ImGui::Selectable(mag_filter_items[n], is_selected))
					{
						App->resources->SetMagFilter(TextureMagFilter(n));
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// Texture wrap combo box
			const char* wrap_items[] = {"Repeat", "Clamp to Edge", "Clamp to Border", "Mirrored Repeat", "Mirrored Clamp to Edge"};
			const char* wrap_item_current = wrap_items[int(App->resources->GetWrap())];
			sprintf_s(name, 50, "Wrap##mwrap_filter_%d", count);
			if (ImGui::BeginCombo(name, wrap_item_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(wrap_items); ++n)
				{
					bool is_selected = (wrap_item_current == wrap_items[n]);
					if (ImGui::Selectable(wrap_items[n], is_selected))
					{
						App->resources->SetWrap(TextureWrap(n));
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Separator();
			if (material->material.diffuse_map != nullptr)
			{
				ImGui::TextColored(App->editor->title_color, "Diffuse Texture");
				ImGui::TextWrapped("Size: ");
				ImGui::SameLine();
				int width;
				int height;
				glGetTextureLevelParameteriv(material->material.diffuse_map->gl_texture, 0, GL_TEXTURE_WIDTH, &width);
				glGetTextureLevelParameteriv(material->material.diffuse_map->gl_texture, 0, GL_TEXTURE_HEIGHT, &height);
				ImGui::TextWrapped("%d x %d", width, height);
				ImGui::Image((void*) material->material.diffuse_map->gl_texture, ImVec2(200, 200));
				ImGui::Separator();
			}
			if (material->material.specular_map != nullptr)
			{
				ImGui::TextColored(App->editor->title_color, "Specular Texture");
				ImGui::TextWrapped("Size: ");
				ImGui::SameLine();
				int width;
				int height;
				glGetTextureLevelParameteriv(material->material.specular_map->gl_texture, 0, GL_TEXTURE_WIDTH, &width);
				glGetTextureLevelParameteriv(material->material.specular_map->gl_texture, 0, GL_TEXTURE_HEIGHT, &height);
				ImGui::TextWrapped("%d x %d", width, height);
				ImGui::Image((void*) material->material.specular_map->gl_texture, ImVec2(200, 200));
				ImGui::Separator();
			}
		}
		count++;
	}
}

void ComponentMaterial::Save(JsonValue j_component) const
{
	j_component["HasDiffuseMap"] = material.has_diffuse_map;
	JsonValue j_diffuse_color = j_component["DiffuseColor"];
	j_diffuse_color[0] = material.diffuse_color.x;
	j_diffuse_color[1] = material.diffuse_color.y;
	j_diffuse_color[2] = material.diffuse_color.z;
	if (material.has_diffuse_map) j_component["DiffuseMapFileName"] = material.diffuse_map->file_name.c_str();

	j_component["HasSpecularMap"] = material.has_specular_map;
	JsonValue j_specular_color = j_component["SpecularColor"];
	j_specular_color[0] = material.specular_color.x;
	j_specular_color[1] = material.specular_color.y;
	j_specular_color[2] = material.specular_color.z;
	if (material.has_specular_map) j_component["SpecularMapFileName"] = material.specular_map->file_name.c_str();

	j_component["Shininess"] = material.shininess;
	j_component["HasShininessInAlphaChannel"] = material.has_shininess_in_alpha_channel;

	JsonValue j_ambient = j_component["Ambient"];
	j_ambient[0] = material.ambient.x;
	j_ambient[1] = material.ambient.y;
	j_ambient[2] = material.ambient.z;
}

void ComponentMaterial::Load(JsonValue j_component)
{
	material.has_diffuse_map = j_component["HasDiffuseMap"];
	JsonValue j_diffuse_color = j_component["DiffuseColor"];
	material.diffuse_color.Set(j_diffuse_color[0], j_diffuse_color[1], j_diffuse_color[2]);
	if (material.has_diffuse_map)
	{
		if (material.diffuse_map == nullptr) material.diffuse_map = App->resources->ObtainTexture();

		TextureImporter::UnloadTexture(material.diffuse_map);
		material.diffuse_map->file_name = j_component["DiffuseMapFileName"];
		TextureImporter::LoadTexture(material.diffuse_map);
	}
	else if (material.diffuse_map != nullptr)
	{
		App->resources->ReleaseTexture(material.diffuse_map);
		material.diffuse_map = nullptr;
	}

	material.has_specular_map = j_component["HasSpecularMap"];
	JsonValue j_specular_color = j_component["SpecularColor"];
	material.specular_color.Set(j_specular_color[0], j_specular_color[1], j_specular_color[2]);
	if (material.has_specular_map)
	{
		if (material.specular_map == nullptr) material.specular_map = App->resources->ObtainTexture();

		TextureImporter::UnloadTexture(material.specular_map);
		material.specular_map->file_name = j_component["SpecularMapFileName"];
		TextureImporter::LoadTexture(material.specular_map);
	}
	else if (material.specular_map != nullptr)
	{
		App->resources->ReleaseTexture(material.specular_map);
		material.specular_map = nullptr;
	}

	material.shininess = j_component["Shininess"];
	material.has_shininess_in_alpha_channel = j_component["HasShininessInAlphaChannel"];

	JsonValue j_ambient = j_component["Ambient"];
	material.ambient.Set(j_ambient[0], j_ambient[1], j_ambient[2]);
}
