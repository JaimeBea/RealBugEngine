#include "ComponentLight.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui.h"

ComponentLight::ComponentLight(GameObject& owner) : Component(static_type, owner) {}

void ComponentLight::OnEditor() {

	GameObject* selected = App->editor->panel_hierarchy.selected_object;
	std::vector < ComponentLight*> lights = selected->GetComponents<ComponentLight>();
	for (ComponentLight* light : lights)
	{
		int count = 1;
		char name[50];
		sprintf_s(name, 50, "Light %d", count);
		if (light != nullptr)
		{
			// Show only # when multiple
			if (lights.size() == 1)
			{
				sprintf_s(name, 50, "Light");
			}
			if (ImGui::CollapsingHeader(name))
			{
				bool active = this->IsActive();
				if (ImGui::Checkbox("Active##light", &active))
				{
					if (active)
					{
						this->Enable();
					}
					else
					{
						this->Disable();
					}
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
}
