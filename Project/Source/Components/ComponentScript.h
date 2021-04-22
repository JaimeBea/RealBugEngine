#pragma once

#include "Component.h"
#include "Script.h"

#include "FileSystem/JsonValue.h"

class ComponentScript : public Component {
public:
	REGISTER_COMPONENT(ComponentScript, ComponentType::SCRIPT, true);

	void Update();
	void OnStart();

	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const;
	void Load(JsonValue jComponent);

	UID GetScriptID() const;

public:
	bool onGame = false;

private:
	UID scriptID = 0;
};