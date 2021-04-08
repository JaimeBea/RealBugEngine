#include "ComponentScript.h"

#include "Application.h"
#include "Modules/ModuleProject.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Resources/ResourceScript.h"
#include "Utils/FileDialog.h"
#include "Utils/ImGuiUtils.h"

#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_SCRIPT "Script"
#define JSON_TAG_NAME "Name"

void ComponentScript::Init() {
}

void ComponentScript::Update() {
	if (App->time->HasGameStarted()) {
		OnUpdate();
	}
}

void ComponentScript::OnStart() {
	ResourceScript* resource = (ResourceScript*) App->resources->GetResource(id);
	if (resource != nullptr) {
		resource->script->Start();
	}
}

void ComponentScript::OnUpdate() {
	ResourceScript* resource = (ResourceScript*) App->resources->GetResource(id);
	if (resource != nullptr) {
		if (resource->script) {
			resource->script->Update();
		}
	}
}

void ComponentScript::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	UID idAnt = id;
	ImGui::ResourceSlot<ResourceScript>("Script", &id);

	static char name[1024] = "";
	ImGui::InputText("Script name", name, 1024);
	ImGui::SameLine();
	if (ImGui::Button("Create Script")) {
		App->project->CreateScript(std::string(name));
	}
}

void ComponentScript::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SCRIPT] = id;
}

void ComponentScript::Load(JsonValue jComponent) {
	id = jComponent[JSON_TAG_SCRIPT];
	if (id != 0) App->resources->IncreaseReferenceCount(id);
}