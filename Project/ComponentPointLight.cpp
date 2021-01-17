#include "ComponentPointLight.h"

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

void ComponentPointLight::OnTransformUpdate()
{
	light.pos = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
}

void ComponentPointLight::DrawGizmos()
{
	if (IsActive() && draw_gizmos)
	{
		float delta = light.kl * light.kl - 4 * (light.kc - 10) * light.kq;
		float distance = Max(abs((-light.kl + sqrt(delta))) / (2 * light.kq), abs((-light.kl - sqrt(delta)) / (2 * light.kq)));
		dd::sphere(light.pos, dd::colors::White, distance);
	}
}

void ComponentPointLight::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector<ComponentPointLight*> lights = selected->GetComponents<ComponentPointLight>();
	int count = 1;

	for (ComponentPointLight* light : lights)
	{
		// Show only # when multiple
		char name[50];
		if (lights.size() == 1)
		{
			sprintf_s(name, 10, "Light");
		}
		else
		{
			sprintf_s(name, 30, "Light %d#point_light_%d", count, count);
		}

		if (ImGui::CollapsingHeader(name))
		{
			bool active = IsActive();
			if (ImGui::Checkbox("Active##point_light_active", &active))
			{
				active ? Enable() : Disable();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##point_light_remove"))
			{
				// TODO: Fix me
				//selected->RemoveComponent(material);
				//continue;
			}
			ImGui::Separator();

			ImGui::Checkbox("Draw Gizmos##spot_light_gizmos", &draw_gizmos);
			ImGui::Separator();

			ImGui::TextColored(title_color, "Parameters");
			ImGui::ColorEdit3("Color##point_light_color", light->light.color.ptr());
			ImGui::DragFloat("Intensity##point_light_intensity", &light->light.intensity, drag_speed3f, 0.0f, inf);
			ImGui::DragFloat("Linear Constant##point_light_kl", &light->light.kl, drag_speed5f, 0.0f, 2.0f);
			ImGui::DragFloat("Quadratic Constant##point_light_kq", &light->light.kq, drag_speed5f, 0.0f, 2.0f);
		}
	}
}

void ComponentPointLight::Save(JsonValue j_component) const
{
	JsonValue j_color = j_component["Color"];
	j_color[0] = light.color.x;
	j_color[1] = light.color.y;
	j_color[2] = light.color.z;

	JsonValue j_intensity = j_component["Intensity"];
	j_intensity = light.intensity;

	JsonValue j_kl = j_component["Kl"];
	j_kl = light.kl;

	JsonValue j_kq = j_component["Kq"];
	j_kq = light.kq;
}

void ComponentPointLight::Load(JsonValue j_component)
{
	JsonValue j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	JsonValue j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;

	JsonValue j_kl = j_component["Kl"];
	light.kl = j_kl;

	JsonValue j_kq = j_component["Kq"];
	light.kq = j_kq;
}

PointLight& ComponentPointLight::GetLightStruct() const
{
	return light;
}
