#include "PanelProperties.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"

#include "GL/glew.h"
#include "imgui.h"

#include "Leaks.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

PanelProperties::PanelProperties() : Panel("Properties", true) {}

void PanelProperties::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
        /* TODO: Make properties based on current selected node and it's components
        
		// Transformation
		if (ImGui::CollapsingHeader("Transformation"))
		{
			float4x4 model_matrix = App->renderer->current_model_model_matrix;
			float3 position = model_matrix.TranslatePart();
			ImGui::InputFloat3("Position", position.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			float3 rotation = model_matrix.RotatePart().ToEulerXYZ();
			ImGui::InputFloat3("Rotation", rotation.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			float3 scale = model_matrix.ExtractScale();
			ImGui::InputFloat3("Scale", scale.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
		}

		// Geometry
		if (ImGui::CollapsingHeader("Geometry"))
		{
			const Model& model = *App->renderer->current_model;
            unsigned mesh_index = 0;
            for (const Mesh& mesh : model.meshes)
            {
                char mesh_name[32];
                sprintf(mesh_name, "Mesh %i", mesh_index);
                if (ImGui::TreeNode(mesh_name))
                {
                    ImGui::Text("Vertices:");
                    ImGui::SameLine();
                    ImGui::TextColored(yellow, "%i", mesh.num_vertices);
                    ImGui::Text("Triangles:");
                    ImGui::SameLine();
                    ImGui::TextColored(yellow, "%i", mesh.num_indices / 3);
                    ImGui::Text("Material:");
                    ImGui::SameLine();
                    ImGui::TextColored(yellow, "%i", mesh.material_index);

                    ImGui::TreePop();
                }

                mesh_index += 1;
            }

            vec center = model.bounding_sphere.pos;
            float radius = model.bounding_sphere.r;
            ImGui::InputFloat3("Bounding Sphere Pos", center.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Bounding Sphere radius", &radius, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}

		// Textures
		if (ImGui::CollapsingHeader("Textures"))
        {
            const Model& model = *App->renderer->current_model;
            unsigned material_index = 0;
            for (unsigned material : model.materials)
            {
                char material_name[32];
                sprintf(material_name, "Material %i", material_index);
                if (ImGui::TreeNode(material_name))
                {
                    int width;
                    int height;
                    glGetTextureLevelParameteriv(model.materials[material_index], 0, GL_TEXTURE_WIDTH, &width);
                    glGetTextureLevelParameteriv(model.materials[material_index], 0, GL_TEXTURE_HEIGHT, &height);

                    ImGui::Text("Width: ");
                    ImGui::SameLine();
                    ImGui::TextColored(yellow, "%i", width);
                    ImGui::Text("Height: ");
                    ImGui::SameLine();
                    ImGui::TextColored(yellow, "%i", height);
                    ImGui::Image((void*)model.materials[material_index], ImVec2(200, 200));

                    ImGui::TreePop();
                }

                material_index += 1;
            }
		}
        */
	}
	ImGui::End();
}
