#include "ComponentScript.h"

#include "Application.h"
#include "Modules/ModuleProject.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleScene.h"
#include "Resources/ResourceScript.h"
#include "Utils/FileDialog.h"
#include "Utils/ImGuiUtils.h"
#include "GameObject.h"
#include "Script.h"

#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_SCRIPT "Script"
#define JSON_TAG_NAME "Name"

void ComponentScript::Update() {
	ReloadScript();

	if (App->time->HasGameStarted() && App->scene->sceneLoaded) {
		if (scriptInstance != nullptr) {
			scriptInstance->Update();
		}
	}
}

void ComponentScript::OnStart() {
	ReloadScript();

	if (scriptInstance != nullptr) {
		scriptInstance->Start();
	}
}

void ComponentScript::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	UID oldScriptId = scriptId;
	ImGui::ResourceSlot<ResourceScript>("Script", &scriptId);
	if (oldScriptId != scriptId) {
		dirty = true;
	}

	static char name[1024] = "";
	ImGui::InputText("Script name", name, 1024);
	ImGui::SameLine();
	if (ImGui::Button("Create Script")) {
		App->project->CreateScript(std::string(name));
	}
}

void ComponentScript::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SCRIPT] = scriptId;
}

void ComponentScript::Load(JsonValue jComponent) {
	scriptId = jComponent[JSON_TAG_SCRIPT];
	if (scriptId != 0) App->resources->IncreaseReferenceCount(scriptId);
	dirty = true;
}

void ComponentScript::DuplicateComponent(GameObject& owner) {
	ComponentScript* component = owner.CreateComponent<ComponentScript>();
	component->scriptId = scriptId;
	component->dirty = true;
}

Script* ComponentScript::GetScriptInstance() {
	ReloadScript();
	return scriptInstance;
}

void ComponentScript::ReloadScript() {
	if (dirty) {
		RELEASE(scriptInstance);
		ResourceScript* script = App->resources->GetResource<ResourceScript>(scriptId);
		if (script != nullptr) {
			scriptInstance = Factory::Create(script->name);
			dirty = false;
		}
	}
}