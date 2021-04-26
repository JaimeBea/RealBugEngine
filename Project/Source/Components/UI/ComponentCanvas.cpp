#include "ComponentCanvas.h"

#include "Application.h"
#include "GameObject.h"
#include "ComponentCanvasRenderer.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "imgui.h"

#include "Utils/Leaks.h"

void ComponentCanvas::Init() {
	dirty = true;
}

void ComponentCanvas::Save(JsonValue jComponent) const {
}

void ComponentCanvas::Load(JsonValue jComponent) {
}

void ComponentCanvas::DuplicateComponent(GameObject& owner) {
	ComponentCanvas* component = owner.CreateComponent<ComponentCanvas>();
	component->screenReferenceSize = screenReferenceSize;
	dirty = true;
}

void ComponentCanvas::SetScreenReferenceSize(float2 screenReferenceSize_) {
	screenReferenceSize = screenReferenceSize_;
}

float ComponentCanvas::GetScreenFactor() {
	if (dirty) {
		dirty = false;
		RecalculateScreenFactor();
	}

	return screenFactor;
}

void ComponentCanvas::SetDirty(bool dirty_) {
	dirty = dirty_;
}

void ComponentCanvas::RecalculateScreenFactor() {
	float2 factor = float2(App->renderer->viewportWidth, App->renderer->viewportHeight).Div(screenReferenceSize);
	screenFactor = factor.x < factor.y ? factor.x : factor.y;
}

void ComponentCanvas::OnEditorUpdate() {
	float2 refSize = screenReferenceSize;

	if (ImGui::InputFloat2("Reference Screen Size", refSize.ptr(), "%.0f")) {
		SetScreenReferenceSize(refSize);
	}
}
