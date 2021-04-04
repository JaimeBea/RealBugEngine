#pragma once

#include "Component.h"
#include "Script.h"

#include "FileSystem/JsonValue.h"

class ComponentScript : public Component {
public:
	REGISTER_COMPONENT(ComponentScript, ComponentType::SCRIPT, true);

	void Init();
	void Update();

	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const;
	void Load(JsonValue jComponent);

private:
	std::string name = "";
	Script* script = nullptr;
	UID id = 0;
};