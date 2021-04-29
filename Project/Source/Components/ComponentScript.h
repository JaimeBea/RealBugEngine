#pragma once

#include "Component.h"

class Script;

class ComponentScript : public Component {
public:
	REGISTER_COMPONENT(ComponentScript, ComponentType::SCRIPT, true);

	void Update() override;
	void OnStart();
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	Script* GetScriptInstance();

private:
	void ReloadScript();

public:
	bool dirty = true;

private:
	UID scriptId = 0;
	Script* scriptInstance = nullptr;
};