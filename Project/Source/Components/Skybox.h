#pragma once
#include <vector>
#include <string>
#include "Application.h"

#include "Resources/ResourceSkybox.h"
#include "Resources/ResourceShader.h"
#include "Modules/ModuleResources.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include <FileSystem/JsonValue.h>

class Skybox {
public:
	Skybox();
	virtual ~Skybox();
	void Draw();
	unsigned int loadCubemap(std::vector<std::string> files);

	void Load(JsonValue jComponent);

private:
	ResourceSkybox* skybox;
	ResourceShader* shader;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int vbo, vao, ebo;
	unsigned int programSky;
	unsigned int glCubeMap;
};
