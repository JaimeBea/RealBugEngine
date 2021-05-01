#include "ComponentToggle.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Components/UI/ComponentImage.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Application.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceScript.h"
#include "Script.h"

#include "Utils/Leaks.h"

#define JSON_TAG_IS_ON "IsOn"
#define JSON_TAG_ENABLED_IMAGE_ID "EnabledImageID"

ComponentToggle ::~ComponentToggle() {}

void ComponentToggle::Init() {
	OnValueChanged();
}

void ComponentToggle::OnClicked() {
	SetOn(!IsOn());
	OnValueChanged();

	App->userInterface->GetCurrentEventSystem()->SetSelected(GetOwner().GetComponent<ComponentSelectable>()->GetID());

	for (ComponentScript& scriptComponent : GetOwner().GetComponents<ComponentScript>()) {
		ResourceScript* scriptResource = App->resources->GetResource<ResourceScript>(scriptComponent.GetScriptID());
		if (scriptResource != nullptr) {
			Script* script = scriptResource->script;
			if (script != nullptr) {
				script->OnButtonClick();
			}
		}
	}
}

void ComponentToggle ::OnValueChanged() {
	ComponentImage* childImage = GetEnabledImage();
	if (childImage) {
		if (IsOn()) {
			childImage->Enable();
		} else {
			childImage->Disable();
		}
	}
	LOG("Toggle value changed");
}

bool ComponentToggle ::IsOn() const {
	return isOn;
}
void ComponentToggle ::SetOn(bool b) {
	isOn = b;
	OnValueChanged();
}

ComponentImage* ComponentToggle::GetEnabledImage() const {
	return (ComponentImage*) App->scene->scene->GetComponent<ComponentImage>(enabledImageID);
}

void ComponentToggle::SetEnabledImage(ComponentImage* enabledImage_) {
	if (enabledImage_) {
		enabledImageID = enabledImage_->GetID();
	}
}

void ComponentToggle::DuplicateComponent(GameObject& owner) {
	ComponentToggle* component = owner.CreateComponent<ComponentToggle>();
	component->isOn = isOn;
}

void ComponentToggle::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_IS_ON] = isOn;
	jComponent[JSON_TAG_ENABLED_IMAGE_ID] = enabledImageID;
}

void ComponentToggle::Load(JsonValue jComponent) {
	isOn = jComponent[JSON_TAG_IS_ON];
	enabledImageID = jComponent[JSON_TAG_ENABLED_IMAGE_ID];
}
