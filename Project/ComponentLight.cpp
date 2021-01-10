#include "ComponentLight.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui.h"

void ComponentLight::OnEditorUpdate()
{
	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector<ComponentLight*> lights = selected->GetComponents<ComponentLight>();
	int count = 1;

	for (ComponentLight* light : lights)
	{
		// Show only # when multiple
		char name[50];
		if (lights.size() == 1)
		{
			sprintf_s(name, 50, "Light");
		}
		else
		{
			sprintf_s(name, 50, "Light %d", count);
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

			ImGui::TextColored(title_color, "Color");
			ImGui::ColorEdit3("##light_color", light->light.color.ptr());
		}
	}
}

void ComponentLight::Save(JsonValue& j_component) const
{
	JsonValue& j_position = j_component["Position"];
	j_position[0] = light.position.x;
	j_position[1] = light.position.y;
	j_position[2] = light.position.z;

	JsonValue& j_color = j_component["Color"];
	j_color[0] = light.color.x;
	j_color[1] = light.color.y;
	j_color[2] = light.color.z;
}

void ComponentLight::Load(const JsonValue& j_component)
{
	const JsonValue& j_position = j_component["Position"];
	light.position.Set(j_position[0], j_position[1], j_position[2]);

	const JsonValue& j_color = j_component["Color"];
	light.color.Set(j_color[0], j_color[1], j_color[2]);
}
