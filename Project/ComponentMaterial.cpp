#include "ComponentMaterial.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "ComponentLight.h"

#include "imgui.h"
#include "GL/glew.h"

ComponentMaterial::ComponentMaterial(GameObject& owner)
	: Component(static_type, owner) {}

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
			bool active = this->IsActive();
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
			}
			if (material->material_type == ShaderType::PHONG)
			{
				ImGui::DragFloat("Kd", &material->Kd, drag_speed3f, 0.0f, 1.0f);
				ImGui::DragFloat("Ks", &material->Ks, drag_speed3f, 0.0f, 1.0f);
				ImGui::DragInt("n", &material->n, 0.05f, 1, 1000);
			}
			ImGui::Separator();
			ImGui::TextColored(title_color, "Filters");

			// Min filter combo box
			const char* min_filter_items[] = {"Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear"};
			const char* min_filter_item_current = min_filter_items[int(App->textures->GetMinFilter())];
			if (ImGui::BeginCombo("Min filter", min_filter_item_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(min_filter_items); ++n)
				{
					bool is_selected = (min_filter_item_current == min_filter_items[n]);
					if (ImGui::Selectable(min_filter_items[n], is_selected))
					{
						App->textures->SetMinFilter(TextureMinFilter(n));
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
			const char* mag_filter_item_current = mag_filter_items[int(App->textures->GetMagFilter())];
			if (ImGui::BeginCombo("Mag filter", mag_filter_item_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(mag_filter_items); ++n)
				{
					bool is_selected = (mag_filter_item_current == mag_filter_items[n]);
					if (ImGui::Selectable(mag_filter_items[n], is_selected))
					{
						App->textures->SetMagFilter(TextureMagFilter(n));
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
			const char* wrap_item_current = wrap_items[int(App->textures->GetWrap())];
			if (ImGui::BeginCombo("Wrap", wrap_item_current))
			{
				for (int n = 0; n < IM_ARRAYSIZE(wrap_items); ++n)
				{
					bool is_selected = (wrap_item_current == wrap_items[n]);
					if (ImGui::Selectable(wrap_items[n], is_selected))
					{
						App->textures->SetWrap(TextureWrap(n));
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
			glGetTextureLevelParameteriv(material->texture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(material->texture, 0, GL_TEXTURE_HEIGHT, &height);
			ImGui::TextWrapped("%d x %d", width, height);
			ImGui::Image((void*) material->texture, ImVec2(200, 200));
			ImGui::Separator();
		}
		count++;
	}
}
