#include "PanelInspector.h"

#include "Application.h"
#include "GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentType.h"
#include "Modules/ModuleEditor.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"

#include "GL/glew.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"

#include "Utils/Leaks.h"

PanelInspector::PanelInspector()
	: Panel("Inspector", true) {}

void PanelInspector::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockRightId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_CUBE " ") + name;
	std::string optionsSymbol = std::string(ICON_FK_COG);
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		GameObject* selected = App->editor->selectedGameObject;
		if (selected != nullptr) {
			ImGui::TextUnformatted("Id:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%llu", selected->GetID());

			bool active = selected->IsActive();
			if (ImGui::Checkbox("##game_object", &active)) {
				// TODO: EventSystem would generate an event here
				if (active) {
					selected->Enable();
				} else {
					selected->Disable();
				}
			}
			ImGui::SameLine();
			char name[100];
			sprintf_s(name, 100, "%s", selected->name.c_str());
			if (ImGui::InputText("", name, 100)) {
				selected->name = name;
			}

			// TODO: Fix Me
			ImGui::SameLine();
			HelpMarker("To Fix it.");

			ImGui::Separator();

			// Show Component info
			std::string cName = "";
			for (Component* component : selected->GetComponents()) {
				switch (component->GetType()) {
				case ComponentType::TRANSFORM:
					cName = "Transformation";
					break;
				case ComponentType::MESH_RENDERER:
					cName = "Mesh Renderer";
					break;
				case ComponentType::CAMERA:
					cName = "Camera";
					break;
				case ComponentType::LIGHT:
					cName = "Light";
					break;
				case ComponentType::BOUNDING_BOX:
					cName = "Bounding Box";
					break;
				case ComponentType::TRANSFORM2D:
					cName = "Rect Transform";
					break;
				case ComponentType::BOUNDING_BOX_2D:
					cName = "Bounding Box 2D";
					break;
				case ComponentType::EVENT_SYSTEM:
					cName = "Event System";
					break;
				case ComponentType::IMAGE:
					cName = "Image";
					break;
				case ComponentType::CANVAS:
					cName = "Canvas";
					break;
				case ComponentType::CANVASRENDERER:
					cName = "Canvas Renderer";
					break;
				case ComponentType::BUTTON:
					cName = "Button";
					break;
				case ComponentType::TOGGLE:
					cName = "Toggle";
					break;
				default:
					cName = "";
					break;
				}

				ImGui::PushID(component);

				// TODO: Place TransformComponent always th the top of the Inspector
				bool headerOpen = ImGui::CollapsingHeader(cName.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

				// Options BUTTON (in the same line and at the end of the line)
				ImGui::SameLine();
				if (ImGui::GetWindowWidth() > 170) ImGui::Indent(ImGui::GetWindowWidth() - 43);
				if (ImGui::Button(optionsSymbol.c_str())) ImGui::OpenPopup("Component Options");
				// More Component buttons (edit the Indention)...
				if (ImGui::GetWindowWidth() > 170) ImGui::Unindent(ImGui::GetWindowWidth() - 43);

				// Options POPUP
				if (ImGui::BeginPopup("Component Options")) {
					if (component->GetType() != ComponentType::TRANSFORM) {
						if (ImGui::MenuItem("Remove Component")) componentToDelete = component;
						// TODO: force remove other components that this one requires for functioning
					}
					// More Options items ...
					ImGui::EndPopup();
				}

				// Show Component info
				if (headerOpen) component->OnEditorUpdate();

				ImGui::PopID();
			}

			// Add New Components
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
			if (ImGui::Button("Add New Component", ImVec2(ImGui::GetContentRegionAvail().x, 25))) { ImGui::OpenPopup("AddComponentPopup"); }
			if (ImGui::BeginPopup("AddComponentPopup")) {
				// Add a Component of type X. If a Component of the same type exists, it wont be created and the modal COMPONENT_EXISTS will show up.
				if (ImGui::MenuItem("Mesh Renderer")) {
					ComponentMeshRenderer* meshRenderer = selected->CreateComponent<ComponentMeshRenderer>();
					if (meshRenderer != nullptr) {
						meshRenderer->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Camera")) {
					ComponentCamera* camera = selected->CreateComponent<ComponentCamera>();
					if (camera != nullptr) {
						camera->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Light")) {
					ComponentLight* light = selected->CreateComponent<ComponentLight>();
					if (light != nullptr) {
						light->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}

				AddUIComponentsOptions(selected);

				// TRANSFORM is always there, cannot add a new one.
				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}

Component* PanelInspector::GetComponentToDelete() const {
	return componentToDelete;
}

void PanelInspector::SetComponentToDelete(Component* comp) {
	componentToDelete = comp;
}

void PanelInspector::AddUIComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("UI")) {
		bool newUIComponentCreated = false;
		if (ImGui::MenuItem("Image")) {
			ComponentImage* component = selected->CreateComponent<ComponentImage>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Transform 2D")) {
			ComponentTransform2D* component = selected->CreateComponent<ComponentTransform2D>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Canvas")) {
			ComponentCanvas* component = selected->CreateComponent<ComponentCanvas>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Canvas Renderer")) {
			ComponentCanvasRenderer* component = selected->CreateComponent<ComponentCanvasRenderer>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Event System")) {
			ComponentEventSystem* component = selected->CreateComponent<ComponentEventSystem>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (newUIComponentCreated) {
			// TODO: create required components that come along with a UI element
			// if not has the required component, create it 
		}

		ImGui::EndMenu();
	}
}
