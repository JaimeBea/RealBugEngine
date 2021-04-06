#include "ComponentScript.h"
#include "Application.h"
#include "Resources/ResourceScript.h"
#include "Modules/ModuleEventSystem.h"

#include "imgui.h"
#include "Utils/ImGuiUtils.h"

#define JSON_TAG_SCRIPT "Script"
#define JSON_TAG_NAME "Name"

void ComponentScript::Init() {
	script = Factory::create(name);

	//App->eventSystem->AddObserverToEvent(Event::EventType::PRESSED_PLAY, this); TODO: JAIMICO Y LA COMADREJA
}

void ComponentScript::Update() {
	if (onGame) {
		OnUpdate();
	}
}

void ComponentScript::OnStart() {
	assert(script);

	script->Start();

}

void ComponentScript::OnUpdate() {
	assert(script != nullptr);

	script->Update();
}

void ComponentScript::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	ImGui::ResourceSlot<ResourceScript>("Script", &id);
}

void ComponentScript::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SCRIPT] = id;
	jComponent[JSON_TAG_NAME] = name.c_str();
}

void ComponentScript::Load(JsonValue jComponent) {
	id = jComponent[JSON_TAG_SCRIPT];
	name = jComponent[JSON_TAG_NAME];
	if (id != 0) App->resources->IncreaseReferenceCount(id);
}