#include "ComponentSpotLight.h"

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

void ComponentSpotLight::Update()
{
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	// TODO: Fix update direction
	light.direction = float3(0.0f, pi / 2, 0.0f);
}

void ComponentSpotLight::DrawGizmos()
{
	if (IsActive())
	{
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		dd::line(transform->GetPosition(), light.direction * 10000, dd::colors::White);
	}
}

void ComponentSpotLight::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector<ComponentSpotLight*> lights = selected->GetComponents<ComponentSpotLight>();
	int count = 1;

	for (ComponentSpotLight* light : lights)
	{
		// Show only # when multiple
		char name[50];
		if (lights.size() == 1)
		{
			sprintf_s(name, 50, "Light");
		}
		else
		{
			sprintf_s(name, 50, "Light %d##spot_light_%d", count, count);
		}

		if (ImGui::CollapsingHeader(name))
		{
			bool active = IsActive();
			if (ImGui::Checkbox("Active##spot_light_active", &active))
			{
				active ? Enable() : Disable();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##spot_light_remove"))
			{
				// TODO: Fix me
				//selected->RemoveComponent(material);
				//continue;
			}
			ImGui::Separator();

			ImGui::TextColored(title_color, "Parameters");
			ImGui::InputFloat3("Direction##spot_light_direction", light->light.direction.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::ColorEdit3("Color##spot_light_color", light->light.color.ptr());
			ImGui::DragFloat("Intenisty##spot_light_intensity", &light->light.intensity, drag_speed3f, 0.0f, 1.0f);
			ImGui::DragFloat("Linear Constant##spot_light_kl", &light->light.kl, drag_speed3f, 0.0f, 2.0f);
			ImGui::DragFloat("Quadratic Constant##spot_light_kq", &light->light.kq, drag_speed3f, 0.0f, 2.0f);

			float deg_outter_angle = light->light.outter_angle * RADTODEG;
			float deg_inner_angle = light->light.inner_angle * RADTODEG;
			if (ImGui::DragFloat("Outter Angle##spot_light_outer_angle", &deg_outter_angle, drag_speed3f, 0.0f, 90.0f))
			{
				light->light.outter_angle = deg_outter_angle * DEGTORAD;
			}
			if (ImGui::DragFloat("Inner Angle##spot_light_inner_angle", &deg_inner_angle, drag_speed3f, 0.0f, deg_outter_angle))
			{
				light->light.inner_angle = deg_inner_angle * DEGTORAD;
			}
		}
	}
}

void ComponentSpotLight::Save(JsonValue& j_component) const
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

	JsonValue& j_kl = j_component["kl"];
	j_kl = light.kl;

	JsonValue& j_kq = j_component["kq"];
	j_kq = light.kq;

	JsonValue& j_inner_angle = j_component["inner_angle"];
	j_inner_angle = light.inner_angle;

	JsonValue& j_outter_angle = j_component["outter_angle"];
	j_outter_angle = light.outter_angle;
}

void ComponentSpotLight::Load(const JsonValue& j_component)
{
	const JsonValue& j_direction = j_component["Direction"];
	light.direction.Set(j_direction[0], j_direction[1], j_direction[2]);

	const JsonValue& j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	const JsonValue& j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;

	const JsonValue& j_kl = j_component["kl"];
	light.kl = j_kl;

	const JsonValue& j_kq = j_component["kq"];
	light.kq = j_kq;

	const JsonValue& j_inner_angle = j_component["inner_angle"];
	light.inner_angle = j_inner_angle;

	const JsonValue& j_outter_angle = j_component["outter_angle"];
	light.outter_angle = j_outter_angle;
}

SpotLight ComponentSpotLight::GetLightStruct() const
{
	return light;
}
