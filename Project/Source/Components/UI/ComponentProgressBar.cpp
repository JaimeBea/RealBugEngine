#include "ComponentProgressBar.h"

#include "GameObject.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentButton.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleUserInterface.h"
#include "Panels/PanelScene.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"

#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#include "Utils/Leaks.h"

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

		rectBack = background->GetComponent<ComponentTransform2D>();
		rectFill = fill->GetComponent<ComponentTransform2D>();
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

	float* val = &value;

	ImGui::TextColored(App->editor->titleColor, "Value");
	if (ImGui::DragFloat("Value", val, App->editor->dragSpeed2f, min, max)) {
		SetValue(*val);
	}
	ImGui::TextColored(App->editor->titleColor, "Min");
	if (ImGui::DragFloat("Min", &min, App->editor->dragSpeed2f, -inf, max - 1)) {
		SetValue(*val);
	}
	ImGui::TextColored(App->editor->titleColor, "Max");
	if (ImGui::DragFloat("Max", &max, App->editor->dragSpeed2f, min + 1, inf)) {
		SetValue(*val);
	}
	ImGui::Checkbox("Right to Left", &rightToLeft);

	ImGui::Separator();
}

void ComponentProgressBar::Save(JsonValue jComponent) const {
}

void ComponentProgressBar::Load(JsonValue jComponent) {
}

void ComponentProgressBar::DuplicateComponent(GameObject& owner) {
}

void ComponentProgressBar::SetValue(float v) {
	value = v;
}
