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

void ComponentSpotLight::OnTransformUpdate()
{
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	// TODO: Fix update direction
	light.pos = transform->GetPosition();
	light.direction = transform->GetRotation() * float3::unitZ;
}

void ComponentSpotLight::DrawGizmos()
{
	if (IsActive() && draw_gizmos)
	{
		dd::cone(light.pos, light.direction * 200, dd::colors::White, 200.0f, 0.0f);
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

			ImGui::Checkbox("Draw Gizmos##spot_light_gizmos", &draw_gizmos);
			ImGui::Separator();

			ImGui::TextColored(title_color, "Parameters");
			ImGui::InputFloat3("Direction##spot_light_direction", light->light.direction.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::ColorEdit3("Color##spot_light_color", light->light.color.ptr());
			ImGui::DragFloat("Intensity##spot_light_intensity", &light->light.intensity, drag_speed3f, 0.0f, inf);
			ImGui::DragFloat("Linear Constant##spot_light_kl", &light->light.kl, drag_speed5f, 0.0f, 2.0f);
			ImGui::DragFloat("Quadratic Constant##spot_light_kq", &light->light.kq, drag_speed5f, 0.0f, 2.0f);

			float deg_outer_angle = light->light.outer_angle * RADTODEG;
			float deg_inner_angle = light->light.inner_angle * RADTODEG;
			if (ImGui::DragFloat("Outter Angle##spot_light_outer_angle", &deg_outer_angle, drag_speed3f, 0.0f, 90.0f))
			{
				light->light.outer_angle = deg_outer_angle * DEGTORAD;
			}
			if (ImGui::DragFloat("Inner Angle##spot_light_inner_angle", &deg_inner_angle, drag_speed3f, 0.0f, deg_outer_angle))
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

	JsonValue& j_kl = j_component["Kl"];
	j_kl = light.kl;

	JsonValue& j_kq = j_component["Kq"];
	j_kq = light.kq;

	JsonValue& j_inner_angle = j_component["InnerAngle"];
	j_inner_angle = light.inner_angle;

	JsonValue& j_outer_angle = j_component["OuterAngle"];
	j_outer_angle = light.outer_angle;
}

void ComponentSpotLight::Load(const JsonValue& j_component)
{
	const JsonValue& j_direction = j_component["Direction"];
	light.direction.Set(j_direction[0], j_direction[1], j_direction[2]);

	const JsonValue& j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	const JsonValue& j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;

	const JsonValue& j_kl = j_component["Kl"];
	light.kl = j_kl;

	const JsonValue& j_kq = j_component["Kq"];
	light.kq = j_kq;

	const JsonValue& j_inner_angle = j_component["InnerAngle"];
	light.inner_angle = j_inner_angle;

	const JsonValue& j_outer_angle = j_component["OuterAngle"];
	light.outer_angle = j_outer_angle;
}

SpotLight& ComponentSpotLight::GetLightStruct() const
{
	return light;
}
