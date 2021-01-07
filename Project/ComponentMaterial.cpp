#include "ComponentMaterial.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "ComponentLight.h"

#include "imgui.h"
#include "GL/glew.h"

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
			sprintf_s(name, 50, "Material %d", count);
		}

		if (ImGui::CollapsingHeader(name))
		{
			bool active = IsActive();
			if (ImGui::Checkbox("Active##material", &active))
			{
				active ? Enable() : Disable();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##material"))
			{
				// TODO: Fix me
				// selected->RemoveComponent(material);
				// continue;
			}
			ImGui::Separator();

			ImGui::TextColored(title_color, "Shader");

			// Material types
			const char* material_types[] = {"Standard", "Phong"};
			const char* material_types_current = material_types[(int) material->material_type];
			if (ImGui::BeginCombo("Type", material_types_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(material_types); ++n)
				{
					bool is_selected = (material_types_current == material_types[n]);
					if (ImGui::Selectable(material_types[n], is_selected))
					{
						material->material_type = ShaderType(n);
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
				ImGui::Text("");
			}
			if (material->material_type == ShaderType::PHONG)
			{
				// Diffuse Texture Combo
				const char* diffuse_items[] = {"Diffuse Color", "Diffuse Texture"};
				const char* diffuse_item_current = diffuse_items[material->has_diffuse_map];
				ImGui::TextColored(text_color, "Diffuse Settings:");
				if (ImGui::BeginCombo("##diffuse", diffuse_item_current))
				{
					for (int n = 0; n < IM_ARRAYSIZE(diffuse_items); ++n)
					{
						bool is_selected = (diffuse_item_current == diffuse_items[n]);
						if (ImGui::Selectable(diffuse_items[n], is_selected))
						{
							material->has_diffuse_map = n ? 1 : 0;
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
					ImGui::ColorEdit3("Color##diffuse_color", material->diffuse_color.ptr());
				}
				ImGui::Text("");

				// Specular Texture Combo
				const char* specular_items[] = {"Specular Color", "Specular Texture"};
				const char* specular_item_current = specular_items[material->has_specular_map];
				ImGui::TextColored(text_color, "Specular Settings:");
				if (ImGui::BeginCombo("##specular", specular_item_current))
				{
					for (int n = 0; n < IM_ARRAYSIZE(specular_items); ++n)
					{
						bool is_selected = (specular_item_current == specular_items[n]);
						if (ImGui::Selectable(specular_items[n], is_selected))
						{
							material->has_specular_map = n ? 1 : 0;
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
					ImGui::ColorEdit3("Color##specular_color", material->specular_color.ptr());
				}

				// Shininess Combo
				const char* shininess_items[] = {"Shininess Value", "Shininess Alpha"};
				const char* shininess_item_current = shininess_items[material->has_shininess_in_alpha_channel];
				if (ImGui::BeginCombo("##shininess", shininess_item_current))
				{
					for (int n = 0; n < IM_ARRAYSIZE(shininess_items); ++n)
					{
						bool is_selected = (shininess_item_current == shininess_items[n]);
						if (ImGui::Selectable(shininess_items[n], is_selected))
						{
							material->has_shininess_in_alpha_channel = n ? 1 : 0;
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
					ImGui::DragFloat("Shininess", &material->shininess, drag_speed3f, 0.0f, 1000.0f);
				}
			}
			ImGui::Separator();
			ImGui::TextColored(title_color, "Filters");

			// Min filter combo box
			const char* min_filter_items[] = {"Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear"};
			const char* min_filter_item_current = min_filter_items[int(App->resources->GetMinFilter())];
			if (ImGui::BeginCombo("Min filter", min_filter_item_current))
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
			if (ImGui::BeginCombo("Mag filter", mag_filter_item_current))
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
			if (ImGui::BeginCombo("Wrap", wrap_item_current))
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
			ImGui::TextColored(title_color, "Texture");
			ImGui::TextWrapped("Size: ");
			ImGui::SameLine();
			int width;
			int height;
			glGetTextureLevelParameteriv(material->diffuse_map->gl_texture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(material->diffuse_map->gl_texture, 0, GL_TEXTURE_HEIGHT, &height);
			ImGui::TextWrapped("%d x %d", width, height);
			ImGui::Image((void*) material->diffuse_map->gl_texture, ImVec2(200, 200));
			ImGui::Separator();
		}
		count++;
	}
}

void ComponentMaterial::Save(JsonValue& j_component) const
{
	j_component["HasDiffuseMap"] = has_diffuse_map;
	JsonValue& j_diffuse_color = j_component["DiffuseColor"];
	j_diffuse_color[0] = diffuse_color.x;
	j_diffuse_color[1] = diffuse_color.y;
	j_diffuse_color[2] = diffuse_color.z;
	if (has_diffuse_map) j_component["DiffuseMapFileId"] = diffuse_map->file_id;

	j_component["HasSpecularMap"] = has_specular_map;
	JsonValue& j_specular_color = j_component["SpecularColor"];
	j_specular_color[0] = specular_color.x;
	j_specular_color[1] = specular_color.y;
	j_specular_color[2] = specular_color.z;
	if (has_specular_map) j_component["SpecularMapFileId"] = specular_map->file_id;

	j_component["Shininess"] = shininess;
	j_component["HasShininessInAlphaChannel"] = has_shininess_in_alpha_channel;

	JsonValue& j_ambient = j_component["Ambient"];
	j_ambient[0] = ambient.x;
	j_ambient[1] = ambient.y;
	j_ambient[2] = ambient.z;
}

void ComponentMaterial::Load(const JsonValue& j_component)
{
	has_diffuse_map = j_component["HasDiffuseMap"];
	const JsonValue& j_diffuse_color = j_component["DiffuseColor"];
	diffuse_color.Set(j_diffuse_color[0], j_diffuse_color[1], j_diffuse_color[2]);
	if (has_diffuse_map)
	{
		if (diffuse_map == nullptr) diffuse_map = App->resources->ObtainTexture();

		App->resources->UnloadTexture(diffuse_map);
		diffuse_map->file_id = j_component["DiffuseMapFileId"];
		App->resources->LoadTexture(diffuse_map);
	}
	else if (diffuse_map != nullptr)
	{
		App->resources->ReleaseTexture(diffuse_map);
		diffuse_map = nullptr;
	}

	has_specular_map = j_component["HasSpecularMap"];
	const JsonValue& j_specular_color = j_component["SpecularColor"];
	specular_color.Set(j_specular_color[0], j_specular_color[1], j_specular_color[2]);
	if (has_specular_map)
	{
		if (specular_map == nullptr) specular_map = App->resources->ObtainTexture();

		App->resources->UnloadTexture(specular_map);
		specular_map->file_id = j_component["SpecularMapFileId"];
		App->resources->LoadTexture(specular_map);
	}
	else if (specular_map != nullptr)
	{
		App->resources->ReleaseTexture(specular_map);
		specular_map = nullptr;
	}

	shininess = j_component["Shininess"];
	has_shininess_in_alpha_channel = j_component["HasShininessInAlphaChannel"];

	const JsonValue& j_ambient = j_component["Ambient"];
	ambient.Set(j_ambient[0], j_ambient[1], j_ambient[2]);
}
