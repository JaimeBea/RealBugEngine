#include "ResourceSkybox.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModulePrograms.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/Leaks.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

#include <iostream>

#define JSON_TAG_SHADER "Shader"

ResourceSkybox::ResourceSkybox(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceSkybox::Load() {
	std::string filePath = GetResourceFilePath();
	LOG("Importing material from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	Buffer<char> buffer = App->files->Load(filePath.c_str());
	if (buffer.Size() == 0) return;

	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return;
	}
	JsonValue jSkybox(document, document);
	//TODO ASSIGN RESOURCE SHADER
	//shader = (ResourceShader*) App->resources->GetResourceByID(jSkybox[JSON_TAG_SHADER]);
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

	ILboolean success = false;

	ilGenImages(1, &glCubeMap);
	ilBindImage(glCubeMap);
	ilEnable(IL_ORIGIN_SET);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	glGenTextures(1, &glCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glCubeMap);
	for (unsigned int i = 0; i < 6; i++) {
		std::string file = jSkybox[JSON_TAG_SHADER][i];
		success = ilLoad(IL_DDS, file.c_str());
		if (success) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		} else {
			std::cout << "Cubemap tex failed to load at path: " << file << std::endl;
		}
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	ilDeleteImage(glCubeMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glDeleteTextures(1,textureID);
}

void ResourceSkybox::Unload() {
	glDeleteBuffers(1, &skyboxVBO);
}