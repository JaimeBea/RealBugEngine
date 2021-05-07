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
#define SLIDER_WIDTH 400.0f
#define HANDLE_WIDTH 20.0f

#define JSON_TAG_COLOR_CLICK "ColorClick"
#define JSON_TAG_MAX_VALUE "MaxValue"
#define JSON_TAG_MIN_VALUE "MinValue"
#define JSON_TAG_CURRENT_VALUE "CurrentValue"
#define JSON_TAG_DIRECTION "Direction"

ComponentSlider::~ComponentSlider() {

}

void ComponentSlider::Init() {
	// TODO: Refactor this. It's not good right now but it let's me check the functionality + Initialization currently broken
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
	//SetNormalizedValue();
}

void ComponentSlider::Update() {

	if (clicked) {
		if (!App->input->GetMouseButton(1)) {
			clicked = false;
		} else {
			float2 mousePos = App->input->GetMousePosition(true);
			float2 auxNewPosition = float2(mousePos.x * 2 - App->renderer->GetViewportSize().x, mousePos.y * 2 - App->renderer->GetViewportSize().y);
			if (newPosition.x != auxNewPosition.x) {
				newPosition = auxNewPosition;
				OnValueChanged();
			}
		}
	}

	ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* fillTransform = fill->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();

	// Calculate fill area and position

	fillTransform->SetSize(float2(backgroundTransform->GetSize().x * normalizedValue, backgroundTransform->GetSize().y));
	float fillPosition = backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f + (backgroundTransform->GetSize().x * normalizedValue) / 2.0f;
	fillTransform->SetPosition(float3(fillPosition, backgroundTransform->GetPosition().y, backgroundTransform->GetPosition().z));

	// Calculate handle position. WIP

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
	SetClicked(true);
	float2 mousePos = App->input->GetMousePosition(true);
	newPosition = float2(mousePos.x * 2 - App->renderer->GetViewportSize().x, mousePos.y * 2 - App->renderer->GetViewportSize().y);
	LOG("New position: %.f", newPosition.x);
	OnValueChanged();
}

// WIP
void ComponentSlider::OnValueChanged() {
	ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
	float size = (backgroundTransform->GetPosition().x + newPosition.x) - (backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f);
	if (size > backgroundTransform->GetSize().x) {
		size = backgroundTransform->GetSize().x;
	}
	if (size < 0) {
		size = 0;
	}
	normalizedValue = size / backgroundTransform->GetSize().x;
	LOG("Normalized value: %.f", normalizedValue);
}

void ComponentSlider::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_MIN_VALUE] = minValue;
	jComponent[JSON_TAG_MAX_VALUE] = maxValue;
	jComponent[JSON_TAG_CURRENT_VALUE] = currentValue;
	jComponent[JSON_TAG_DIRECTION] = (int) direction;

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	jColorClick[0] = colorClicked.x;
	jColorClick[1] = colorClicked.y;
	jColorClick[2] = colorClicked.z;
	jColorClick[3] = colorClicked.w;

}

void ComponentSlider::Load(JsonValue jComponent) {
	maxValue = jComponent[JSON_TAG_MAX_VALUE];
	minValue = jComponent[JSON_TAG_MIN_VALUE];
	currentValue = jComponent[JSON_TAG_CURRENT_VALUE];
	int dir =  jComponent[JSON_TAG_DIRECTION];
	direction = (DirectionType) dir;
	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
}

void ComponentSlider::DuplicateComponent(GameObject& owner) {

}

bool ComponentSlider::IsClicked() const {
	return clicked;
}

void ComponentSlider::SetClicked(bool clicked_) {
	clicked = clicked_;
}

float4 ComponentSlider::GetClickColor() const {
	return colorClicked;
}

float4 ComponentSlider::GetTintColor() const {
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
