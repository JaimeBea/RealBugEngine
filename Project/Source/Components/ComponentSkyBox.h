#pragma once
#include <vector>
#include <string>
#include "Component.h"

#include "Resources/ResourceSkybox.h"
#include "Resources/ResourceShader.h"
#include "Modules/ModuleResources.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include <FileSystem/JsonValue.h>

class ComponentSkyBox : public Component {
public:
	REGISTER_COMPONENT(ComponentSkyBox, ComponentType::SKYBOX);

	void OnTransformUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void Draw();
	unsigned int loadCubemap(std::vector<std::string> files);
	void Load(JsonValue jComponent);

private:
	ResourceSkybox* skybox;
	unsigned int glCubeMap;
};
