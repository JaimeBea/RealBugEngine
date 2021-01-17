#include "ComponentSpotLight.h"

#include "Globals.h"
#include "Logging.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "PanelInspector.h"

#include "debugdraw.h"
#include "Math/float3x3.h"
#include "imgui.h"

#include "Leaks.h"

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
		float delta = light.kl * light.kl - 4 * (light.kc - 10) * light.kq;
		float distance = Max(abs((-light.kl + sqrt(delta))) / (2 * light.kq), abs((-light.kl - sqrt(delta)) / (2 * light.kq)));
		float b = distance * tan(light.outer_angle);
		dd::cone(light.pos, light.direction * distance, dd::colors::White, distance * tan(light.outer_angle), 0.0f);
	}
}

void ComponentSpotLight::OnEditorUpdate()
{
	if (ImGui::CollapsingHeader("Light"))
	{
		bool active = IsActive();
		if (ImGui::Checkbox("Active", &active))
		{
			active ? Enable() : Disable();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			// TODO: Fix me
			//selected->RemoveComponent(material);
			//continue;
		}
		ImGui::Separator();

		ImGui::Checkbox("Draw Gizmos", &draw_gizmos);
		ImGui::Separator();

		ImGui::TextColored(App->editor->title_color, "Parameters");
		ImGui::InputFloat3("Direction", light.direction.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::ColorEdit3("Color", light.color.ptr());
		ImGui::DragFloat("Intensity", &light.intensity, App->editor->drag_speed3f, 0.0f, inf);
		ImGui::DragFloat("Linear Constant", &light.kl, App->editor->drag_speed5f, 0.0f, 2.0f);
		ImGui::DragFloat("Quadratic Constant", &light.kq, App->editor->drag_speed5f, 0.0f, 2.0f);

		float deg_outer_angle = light.outer_angle * RADTODEG;
		float deg_inner_angle = light.inner_angle * RADTODEG;
		if (ImGui::DragFloat("Outter Angle", &deg_outer_angle, App->editor->drag_speed3f, 0.0f, 90.0f))
		{
			light.outer_angle = deg_outer_angle * DEGTORAD;
		}
		if (ImGui::DragFloat("Inner Angle", &deg_inner_angle, App->editor->drag_speed3f, 0.0f, deg_outer_angle))
		{
			light.inner_angle = deg_inner_angle * DEGTORAD;
		}
	}
}

void ComponentSpotLight::Save(JsonValue j_component) const
{
	JsonValue j_pos = j_component["Pos"];
	j_pos[0] = light.pos.x;
	j_pos[1] = light.pos.y;
	j_pos[2] = light.pos.z;

	JsonValue j_direction = j_component["Direction"];
	j_direction[0] = light.direction.x;
	j_direction[1] = light.direction.y;
	j_direction[2] = light.direction.z;

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

	JsonValue j_inner_angle = j_component["InnerAngle"];
	j_inner_angle = light.inner_angle;

	JsonValue j_outer_angle = j_component["OuterAngle"];
	j_outer_angle = light.outer_angle;
}

void ComponentSpotLight::Load(JsonValue j_component)
{
	JsonValue j_pos = j_component["Pos"];
	light.pos.Set(j_pos[0], j_pos[1], j_pos[2]);

	JsonValue j_direction = j_component["Direction"];
	light.direction.Set(j_direction[0], j_direction[1], j_direction[2]);

	JsonValue j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	JsonValue j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;

	JsonValue j_kl = j_component["Kl"];
	light.kl = j_kl;

	JsonValue j_kq = j_component["Kq"];
	light.kq = j_kq;

	JsonValue j_inner_angle = j_component["InnerAngle"];
	light.inner_angle = j_inner_angle;

	JsonValue j_outer_angle = j_component["OuterAngle"];
	light.outer_angle = j_outer_angle;
}

SpotLight& ComponentSpotLight::GetLightStruct() const
{
	return light;
}
