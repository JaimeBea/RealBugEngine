#include "ComponentPointLight.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Resources/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleEditor.h"

#include "debugdraw.h"
#include "Math/float3x3.h"
#include "imgui.h"

#include "Utils/Leaks.h"

void ComponentPointLight::OnTransformUpdate() {
	light.pos = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
}

void ComponentPointLight::DrawGizmos() {
	if (IsActive() && draw_gizmos) {
		float delta = light.kl * light.kl - 4 * (light.kc - 10) * light.kq;
		float distance = Max(abs((-light.kl + sqrt(delta))) / (2 * light.kq), abs((-light.kl - sqrt(delta)) / (2 * light.kq)));
		dd::sphere(light.pos, dd::colors::White, distance);
	}
}

void ComponentPointLight::OnEditorUpdate() {
	if (ImGui::CollapsingHeader("Light")) {
		bool active = IsActive();
		if (ImGui::Checkbox("Active", &active)) {
			active ? Enable() : Disable();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove")) {
			// TODO: Fix me
			//selected->RemoveComponent(material);
			//continue;
		}
		ImGui::Separator();

		ImGui::Checkbox("Draw Gizmos", &draw_gizmos);
		ImGui::Separator();

		ImGui::TextColored(App->editor->title_color, "Parameters");
		ImGui::ColorEdit3("Color", light.color.ptr());
		ImGui::DragFloat("Intensity", &light.intensity, App->editor->drag_speed3f, 0.0f, inf);
		ImGui::DragFloat("Linear Constant", &light.kl, App->editor->drag_speed5f, 0.0f, 2.0f);
		ImGui::DragFloat("Quadratic Constant", &light.kq, App->editor->drag_speed5f, 0.0f, 2.0f);
	}
}

void ComponentPointLight::Save(JsonValue j_component) const {
	JsonValue j_pos = j_component["Pos"];
	j_pos[0] = light.pos.x;
	j_pos[1] = light.pos.y;
	j_pos[2] = light.pos.z;

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

void ComponentPointLight::Load(JsonValue j_component) {
	JsonValue j_pos = j_component["Pos"];
	light.pos.Set(j_pos[0], j_pos[1], j_pos[2]);

	JsonValue j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);

	JsonValue j_intensity = j_component["Intensity"];
	light.intensity = j_intensity;

	JsonValue j_kl = j_component["Kl"];
	light.kl = j_kl;

	JsonValue j_kq = j_component["Kq"];
	light.kq = j_kq;
}

PointLight& ComponentPointLight::GetLightStruct() const {
	return light;
}
