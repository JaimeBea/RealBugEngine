#pragma once

#include "Component.h"
#include "Resources/ResourceSkybox.h"
#include "Resources/ResourceShader.h"
#include "Modules/ModuleResources.h"
#include "FileSystem/JsonValue.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include <vector>
#include <string>

class ComponentSkyBox : public Component {
public:
	REGISTER_COMPONENT(ComponentSkyBox, ComponentType::SKYBOX, false);

	void OnEditorUpdate() override;
	void Load(JsonValue jComponent) override;
	void Save(JsonValue jComponent) const override;
	void Update() override;
	void Draw();

private:
	UID shaderId = 0;
	UID skyboxId = 0;
};
