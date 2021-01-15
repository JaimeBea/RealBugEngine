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

void ComponentPointLight::DrawGizmos()
{
	if (IsActive())
	{
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		dd::line(transform->GetPosition(), light.direction * 10000, dd::colors::White);
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

			ImGui::TextColored(title_color, "Parameters");
			ImGui::ColorEdit3("Color##point_light_color", light->light.color.ptr());
			ImGui::DragFloat("Intensity##point_light_intensity", &light->light.intensity, drag_speed3f, 0.0f, 1.0f);
			ImGui::DragFloat("Linear Constant##point_light_kl", &light->light.kl, drag_speed3f, 0.0f, 2.0f);
			ImGui::DragFloat("Quadratic Constant##point_light_kq", &light->light.kq, drag_speed3f, 0.0f, 2.0f);
		}
	}
}

void ComponentPointLight::Save(JsonValue& j_component) const
{
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
}

void ComponentPointLight::Load(const JsonValue& j_component)
{
	const JsonValue& j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	const JsonValue& j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;

	const JsonValue& j_kl = j_component["kl"];
	light.kl = j_kl;

	const JsonValue& j_kq = j_component["kq"];
	light.kq = j_kq;
}

PointLight ComponentPointLight::GetLightStruct() const
{
	return light;
}
