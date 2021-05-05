#include "ComponentProgressBar.h"

#include "GameObject.h"
#include "Application.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Modules/ModuleEditor.h"
#include "FileSystem/JsonValue.h"
#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleTime.h"

#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#include "Utils/Leaks.h"

#define JSON_TAG_FILL_POSITION "FillPosition"
#define JSON_TAG_MIN_VALUE "Min"
#define JSON_TAG_MAX_VALUE "Max"
#define JSON_TAG_FILL_DIRECTION "FillDirection"
#define JSON_TAG_PROGRESS_VALUE "ProgressValue"

ComponentProgressBar::~ComponentProgressBar() {
}

void ComponentProgressBar::Init() {
	
}

void ComponentProgressBar::Update() {
	//The progress bar GameObject must have two image gameobjects as childs (for background and fill)
	if (background == nullptr || fill == nullptr) {
		GameObject owner = GetOwner();

		std::vector<GameObject*> childs = owner.GetChildren();

		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it) {
			if (it == childs.begin()) {
				background = *it;
			} else if (fill == nullptr) {
				fill = *it;
			}
		}

		if (background != nullptr && fill != nullptr) {
			rectBack = background->GetComponent<ComponentTransform2D>();
			rectFill = fill->GetComponent<ComponentTransform2D>();
		} else
			return;

	}

	backPos = rectBack->GetPosition();

	backSize = rectBack->GetSize();

	float percent = (value - min) / (max - min);
	//we set the fill image width as the percent of the background according to the value (if 0.5 then 50% of background's width)
	//height stays the same as the back
	rectFill->SetSize(float2((backSize.x * percent), backSize.y));

	//with the size of the fill setted we must position it since if we keep the background pos we will have it centered
	//The image is aligned to the left here, we will give the option to slide from left to right in the future
	fillXPos = ((backSize.x - (backSize.x * percent)) / 2);

	if (!rightToLeft) {
		fillXPos = backPos.x - fillXPos;
		rectFill->SetPosition(float3(fillXPos, backPos.y, backPos.z));
	} else {
		fillXPos = backPos.x + fillXPos;
		rectFill->SetPosition(float3(fillXPos, backPos.y, backPos.z));
	}

}

void ComponentProgressBar::OnEditorUpdate() {

	ImGui::TextColored(App->editor->titleColor, "Value");
	if (ImGui::DragFloat("Value", &value, App->editor->dragSpeed2f, min, max)) {
		SetValue(value);
	}
	ImGui::TextColored(App->editor->titleColor, "Min");
	if (ImGui::DragFloat("Min", &min, App->editor->dragSpeed2f, -inf, max - 1)) {
		SetMin(min);
	}
	ImGui::TextColored(App->editor->titleColor, "Max");
	if (ImGui::DragFloat("Max", &max, App->editor->dragSpeed2f, min + 1, inf)) {
		SetMax(max);
	}
	ImGui::Checkbox("Right to Left", &rightToLeft);

	ImGui::Separator();
}

void ComponentProgressBar::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_FILL_POSITION] = fillXPos;
	jComponent[JSON_TAG_MIN_VALUE] = min;
	jComponent[JSON_TAG_MAX_VALUE] = max;
	jComponent[JSON_TAG_FILL_DIRECTION] = rightToLeft;
	jComponent[JSON_TAG_PROGRESS_VALUE] = value;
}

void ComponentProgressBar::Load(JsonValue jComponent) {
	fillXPos = jComponent[JSON_TAG_FILL_POSITION];
	min = jComponent[JSON_TAG_MIN_VALUE];
	max = jComponent[JSON_TAG_MAX_VALUE];
	rightToLeft = jComponent[JSON_TAG_FILL_DIRECTION];
	value = jComponent[JSON_TAG_PROGRESS_VALUE];
}

void ComponentProgressBar::DuplicateComponent(GameObject& owner) {
	ComponentProgressBar* component = owner.CreateComponent<ComponentProgressBar>();
	component->SetValue(value);
	component->SetFillPos(fillXPos);
	component->SetMin(min);
	component->SetMax(max);
	component->SetFillDir(rightToLeft);
}

void ComponentProgressBar::SetValue(float v) {
	value = v;
}

void ComponentProgressBar::SetFillPos(float fillPos) {
	fillXPos = fillPos;
}

void ComponentProgressBar::SetMin(float m) {
	min = m;
}

void ComponentProgressBar::SetMax(float n) {
	max = n;
}

void ComponentProgressBar::SetFillDir(bool fillDir) {
	rightToLeft = fillDir;
}
