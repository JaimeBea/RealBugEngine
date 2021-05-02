#pragma once

#include "Component.h"
#include "Scripting/Member.h"
#include "Utils/UID.h"

#include <unordered_map>
#include <vector>
#include <string>

class Script;

class ComponentScript : public Component {
public:
	REGISTER_COMPONENT(ComponentScript, ComponentType::SCRIPT, true);

	void Update() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void Invalidate();

	Script* GetScriptInstance();

private:
	void ReloadScriptInstance();

private:
	std::unordered_map<std::string, std::pair<MemberType, MEMBER_VARIANT>> changedValues;

	UID scriptId = 0;
	Script* scriptInstance = nullptr;
	bool dirty = true;
};