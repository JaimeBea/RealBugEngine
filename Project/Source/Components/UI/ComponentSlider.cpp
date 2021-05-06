#include "ComponentSlider.h"

#include "GameObject.h"
#include "Application.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "imgui.h"
#include "Utils/Logging.h"

#include "Utils/Leaks.h"

#define SLIDER_HEIGHT 100.0f
#define SLIDER_WIDTH 200.0f
#define HANDLE_WIDTH 20.0f

ComponentSlider::~ComponentSlider() {

}

void ComponentSlider::Init() {
	// TODO: Refactor this. It's not good right now but it let's me check the functionality
	std::vector<GameObject*> children = GetOwner().GetChildren();
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		if ((*it)->name == "Background") {
			background = *it;
		} else if ((*it)->name == "Fill") {
			fill = *it;
		}
		else if ((*it)->name == "Handle") {
			handle = *it;
		}
	}
	SetDefaultSliderSize();
}

void ComponentSlider::Update() {

	if (clicked) {
		if (!App->input->GetMouseButton(1)) {
			clicked = false;
		}
	}

	ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* fillTransform = fill->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();

	// Calculate fill area and position

	fillTransform->SetSize(float2(backgroundTransform->GetSize().x * normalizedValue, backgroundTransform->GetSize().y));
	float fillPosition = backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f + (backgroundTransform->GetSize().x * normalizedValue) / 2.0f;
	fillTransform->SetPosition(float3(fillPosition, backgroundTransform->GetPosition().y, backgroundTransform->GetPosition().z));

	// Calculate handle position

	float handlePosition = fillTransform->GetPosition().x + (fillTransform->GetSize().x / 2.0f);
	handleTransform->SetPosition(float3(handlePosition, handleTransform->GetPosition().y, handleTransform->GetPosition().z));


}

void ComponentSlider::OnEditorUpdate() {
	float* value = &currentValue;

	if (ImGui::DragFloat("Max. Value", &maxValue, App->editor->dragSpeed1f, minValue + 1, inf)) {
		SetValue(*value);
		SetNormalizedValue();
	}
	if (ImGui::DragFloat("Min. Value", &minValue, App->editor->dragSpeed1f, -inf, maxValue - 1)) {
		SetValue(*value);
		SetNormalizedValue();
	}

	const char* availableDirections[] = {"Left to right", "Right to left", "Up to down", "Down to up"};
	const char* currentDirection = availableDirections[(int) direction];
	if (ImGui::BeginCombo("Direction", currentDirection)) {
		for (int n = 0; n < IM_ARRAYSIZE(availableDirections); ++n) {
			bool isSelected = (currentDirection == availableDirections[n]);
			if (ImGui::Selectable(availableDirections[n], isSelected)) {
				// WIP: For now it does nothing
				direction = DirectionType(n);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::SliderFloat("Value", value, minValue, maxValue)) {
		SetValue(*value);
		SetNormalizedValue();
	}
}

void ComponentSlider::SetValue(float value) {
	if (value > maxValue)
		currentValue = value;
	else if (value < minValue)
		currentValue = minValue;
}

void ComponentSlider::OnClicked() {
	LOG("HEY. SLIDER CLICKED!");
	SetClicked(true);
	float2 mousePos = App->input->GetMousePosition(true);
	LOG("Mouse position: %.f, %.f", mousePos.x, mousePos.y);
	ComponentBoundingBox2D* bb = GetOwner().GetComponent<ComponentBoundingBox2D>();
	LOG("Min x: %.f, Max x: %.f", bb->GetWorldAABB().minPoint.x, bb->GetWorldAABB().maxPoint.x);
}

void ComponentSlider::Save(JsonValue jComponent) const {


}

void ComponentSlider::Load(JsonValue jComponent) {

}

void ComponentSlider::DuplicateComponent(GameObject& owner) {

}

bool ComponentSlider::IsClicked() const {
	return clicked;
}

void ComponentSlider::SetClicked(bool clicked_) {
	clicked = clicked_;
}

const float4& ComponentSlider::GetClickColor() const {
	return colorClicked;
}

const float4& ComponentSlider::GetTintColor() const {
	if (!IsActive()) return float4::one;

	ComponentSelectable* sel = GetOwner().GetComponent<ComponentSelectable>();

	if (!sel) return float4::one;

	if (sel->GetTransitionType() == ComponentSelectable::TransitionType::COLOR_CHANGE) {
		if (!sel->IsInteractable()) {
			return sel->GetDisabledColor();
		} else if (IsClicked()) {
			return colorClicked;
		} else if (sel->IsSelected()) {
			return sel->GetSelectedColor();
		} else if (sel->IsHovered()) {
			return sel->GetHoverColor();
		}
	}

	return float4::one;
}

void ComponentSlider::SetDefaultSliderSize() {
	ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* fillTransform = fill->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* sliderTransform = GetOwner().GetComponent<ComponentTransform2D>();

	sliderTransform->SetSize(float2(SLIDER_WIDTH, SLIDER_HEIGHT));
	backgroundTransform->SetSize(float2(SLIDER_WIDTH, SLIDER_HEIGHT));
	fillTransform->SetSize(float2(backgroundTransform->GetSize().x * normalizedValue, SLIDER_HEIGHT));
	handleTransform->SetSize(float2(HANDLE_WIDTH, SLIDER_HEIGHT));
	handleTransform->SetPosition(float3(sliderTransform->GetPosition().x - normalizedValue, 0.0f, 0.0f));
}

void ComponentSlider::SetNormalizedValue() {
	normalizedValue = (currentValue - minValue) / (maxValue - minValue);
}
