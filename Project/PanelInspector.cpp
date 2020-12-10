#include "PanelInspector.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "PanelHierarchy.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"

#include "GL/glew.h"
#include "imgui.h"

#include "Leaks.h"

static ImVec4 color = ImVec4(0.35f, 0.69f, 0.87f, 1.0f);
static float drag_speed1f = 0.5f;
static float drag_speed2f = 0.05f;
static float drag_speed3f = 0.005f;

PanelInspector::PanelInspector() : Panel("Inspector", true) {}

void PanelInspector::Update()
{
	ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		GameObject* selected = App->editor->panel_hierarchy.selected_object;
		if (selected == nullptr) 
		{
			ImGui::End();
			return;
		}
		char name[100];
		sprintf_s(name, 100, "%s", selected->name.c_str());
		if (ImGui::InputText("Name", name, 100))
		{
			selected->name = name;
		}
		bool enable = true;
		if (ImGui::Checkbox("Enable", &enable))
		{
			// TODO: Add enable option to game Objects
		}

		ImGui::Separator();
		ComponentTransform* transform = selected->GetComponent<ComponentTransform>();
		float3 pos3 = transform->GetPosition();
		float pos[] = { pos3[0], pos3[1], pos3[2] };
		float3 scale3 = transform->GetScale();
		float scale[] = { scale3[0], scale3[1], scale3[2] };
		float3 rotation3 = transform->GetRotation().ToEulerXYZ() * RADTODEG;
		float rotation[] = { rotation3[0], rotation3[1], rotation3[2] };

		// Clamp Values
		if (rotation[1] >= 180) rotation[1] = rotation[1] - 360;
		if (rotation[1] <= -180) rotation[1] = rotation[1] + 360;
		if (rotation[0] == -0.0f || rotation[0] == -180.0f || rotation[0] == 180.0f) rotation[0] = 0.0f;
		if (rotation[2] == -0.0f || rotation[2] == -180.0f || rotation[2] == 180.0f) rotation[2] = 0.0f;

		if (ImGui::CollapsingHeader("Transformation")) 
		{
			ImGui::TextColored(color, "Transformation (X,Y,Z)");
			if (ImGui::DragFloat3("Position", pos, drag_speed2f, -inf, inf))
			{
				transform->SetPosition(float3(pos[0], pos[1], pos[2]));
			}
			if (ImGui::DragFloat3("Scale", scale, drag_speed2f, 0, inf))
			{
				transform->SetScale(float3(scale[0], scale[1], scale[2]));
			}
			if (ImGui::DragFloat3("Rotation", rotation, drag_speed2f, -inf, inf))
			{
				transform->SetRotation(Quat::FromEulerXYZ(rotation[0] * DEGTORAD, rotation[1] * DEGTORAD, rotation[2] * DEGTORAD));
			}
		}

		std::vector<ComponentMesh*> meshes = selected->GetComponents<ComponentMesh>();
		for (std::vector<ComponentMesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
		{
			int count = 1;
			char name[50];
			sprintf_s(name, 50, "Mesh Information %d", count);
			if (*it != nullptr)
			{
				// Show only # when multiple
				if (meshes.size() == 1)
				{
					sprintf_s(name, 50, "Mesh Information");
				}
				if (ImGui::CollapsingHeader(name))
				{
					ImGui::TextColored(color, "Geometry");
					ImGui::TextWrapped("Num Vertices: ");
					ImGui::SameLine();
					ImGui::TextColored(color, "%d", (*it)->num_vertices);
					ImGui::TextWrapped("Num Triangles: ");
					ImGui::SameLine();
					ImGui::TextColored(color, "%d", (*it)->num_indices / 3);
					ImGui::Separator();
				}
				count++;
			}
		}
		std::vector < ComponentMaterial*> materials = selected->GetComponents<ComponentMaterial>();
		for (std::vector<ComponentMaterial*>::iterator it = materials.begin(); it != materials.end(); it++)
		{
			int count = 1;
			char name[50];
			sprintf_s(name, 50, "Material Information %d", count);
			if (*it != nullptr)
			{
				// Show only # when multiple
				if (materials.size() == 1)
				{
					sprintf_s(name, 50, "Material Information");
				}
				if (ImGui::CollapsingHeader(name))
				{
					ImGui::TextColored(color, "Shader");

					// Material types
					const char* material_types[] = { "Standard", "Phong" };
					const char* material_types_current = material_types[int((*it)->material_type)];
					if (ImGui::BeginCombo("Type", material_types_current))
					{
						for (int n = 0; n < IM_ARRAYSIZE(material_types); ++n)
						{
							bool is_selected = (material_types_current == material_types[n]);
							if (ImGui::Selectable(material_types[n], is_selected))
							{
								(*it)->material_type = ShaderType(n);
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if ((*it)->material_type == ShaderType::PHONG) 
					{
						ImGui::DragFloat("Kd", &(*it)->Kd, drag_speed3f, 0.0f, 1.0f);
						ImGui::DragFloat("Ks", &(*it)->Ks, drag_speed3f, 0.0f, 1.0f);
						ImGui::DragInt("n", &(*it)->n, 0.05f, 1, 1000);
					}
					ImGui::Separator();
					ImGui::TextColored(color, "Filters");

					// Min filter combo box
					const char* min_filter_items[] = { "Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear" };
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
					const char* mag_filter_items[] = { "Nearest", "Linear" };
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
					const char* wrap_items[] = { "Repeat", "Clamp to Edge", "Clamp to Border", "Mirrored Repeat", "Mirrored Clamp to Edge" };
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
					ImGui::TextColored(color, "Texture");
					ImGui::TextWrapped("Size: ");
					ImGui::SameLine();
					int width;
					int height;
					glGetTextureLevelParameteriv((*it)->texture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv((*it)->texture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("%d x %d", width, height);
					ImGui::Image((void*)(*it)->texture, ImVec2(200, 200));
					ImGui::Separator();
				}
				count++;
			}
		}
	}
	ImGui::End();
}
