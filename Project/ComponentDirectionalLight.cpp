#include "ComponentDirectionalLight.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "debugdraw.h"
#include "Math/float3x3.h"
#include "Globals.h"
#include "Logging.h"

#include "imgui.h"

void ComponentDirectionalLight::OnTransformUpdate()
{
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	light.direction = transform->GetRotation() * float3::unitZ;
}

void ComponentDirectionalLight::DrawGizmos()
{
	if (IsActive() && draw_gizmos)
	{
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		dd::cone(transform->GetPosition(), light.direction * 200, dd::colors::White, 200.0f, 200.0f);
	}
}

void ComponentDirectionalLight::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector<ComponentDirectionalLight*> lights = selected->GetComponents<ComponentDirectionalLight>();
	int count = 1;

	for (ComponentDirectionalLight* light : lights)
	{
		// Show only # when multiple
		char name[50];
		if (lights.size() == 1)
		{
			sprintf_s(name, 50, "Light");
		}
		else
		{
			sprintf_s(name, 50, "Light %d##light_%d", count, count);
		}

		if (ImGui::CollapsingHeader(name))
		{
			bool active = IsActive();
			if (ImGui::Checkbox("Active##light", &active))
			{
				active ? Enable() : Disable();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##light"))
			{
				// TODO: Fix me
				//selected->RemoveComponent(material);
				//continue;
			}
			ImGui::Separator();

			ImGui::Checkbox("Draw Gizmos##spot_light_gizmos", &draw_gizmos);
			ImGui::Separator();

			ImGui::TextColored(title_color, "Parameters");
			ImGui::InputFloat3("Direction##dir_light_direction", light->light.direction.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::ColorEdit3("Color##dir_light_color", light->light.color.ptr());
			ImGui::DragFloat("Intenisty##dir_light_intensity", &light->light.intensity, drag_speed3f, 0.0f, 1.0f);
		}
	}
}

void ComponentDirectionalLight::Save(JsonValue& j_component) const
{
	JsonValue& j_direction = j_component["Direction"];
	j_direction[0] = light.direction.x;
	j_direction[1] = light.direction.y;
	j_direction[2] = light.direction.z;

	JsonValue& j_color = j_component["Color"];
	j_color[0] = light.color.x;
	j_color[1] = light.color.y;
	j_color[2] = light.color.z;

	JsonValue& j_intensity = j_component["Intensity"];
	j_intensity = light.intensity;
}

void ComponentDirectionalLight::Load(const JsonValue& j_component)
{
	const JsonValue& j_direction = j_component["Direction"];
	light.direction.Set(j_direction[0], j_direction[1], j_direction[2]);

	const JsonValue& j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	const JsonValue& j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;
}

DirectionalLight& ComponentDirectionalLight::GetLightStruct() const
{
	return light;
}
