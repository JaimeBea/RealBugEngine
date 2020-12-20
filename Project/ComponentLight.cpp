#include "ComponentLight.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui.h"

ComponentLight::ComponentLight(GameObject& owner)
	: Component(static_type, owner) {}

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

			ImGui::TextColored(title_color, "Direction");
			ImGui::DragFloat3("##light_direction", light_direction.ptr(), drag_speed2f, -inf, +inf);
			ImGui::TextColored(title_color, "Color");
			ImGui::ColorEdit3("##light_color", light_color.ptr());
		}
	}
}
