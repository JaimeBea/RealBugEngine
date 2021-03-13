#include "ResourceSkybox.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModulePrograms.h"
#include "Utils/Logging.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <iostream>
#include <fstream>

#define JSON_TAG_SHADER "shader"
ResourceSkybox::ResourceSkybox(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {
}

void ResourceSkybox::ReadPath() {
	std::string imgPath;
	std::ifstream infile(this->GetAssetFilePath().c_str());
	for (int i = 0; i < 6; i++) {
		getline(infile, imgPath);
		files.push_back(imgPath);
	}
}
void ResourceSkybox::Load(JsonValue jComponent) {
	ReadPath();
	shader = (ResourceShader*) App->resources->GetResourceByID(jComponent[JSON_TAG_SHADER]);
	/*programSky = App->programs->CreateProgram(this->GetAssetFilePath().c_str(), this->GetAssetFilePath().c_str());*/
	programSky = App->programs->CreateProgram("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");
	skyboxVAO = 0;
	skyboxVBO = 0;
	vbo = 0;
	vao = 0;
	ebo = 0;
	programSky = 0;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

	ILboolean success = false;

	ilGenImages(1, &glCubeMap);
	// bind it
	ilBindImage(glCubeMap);
	// match image origin to OpenGLs
	ilEnable(IL_ORIGIN_SET);
	// ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	glGenTextures(1, &glCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glCubeMap);
	for (unsigned int i = 0; i < files.size(); i++) {
		//TODO CHECK IF ALL FILES EXIST OR SOMETHING
		success = ilLoad(IL_DDS, files[i].c_str());
		if (success) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		} else {
			std::cout << "Cubemap tex failed to load at path: " << files[i] << std::endl;
		}
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	ilDeleteImage(glCubeMap); //release from il since we have it in opengl
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glDeleteTextures(1,textureID); //release from il since we have it in opengl
}

void ResourceSkybox::Unload() {
	glDeleteBuffers(1, &vbo);
}

void ResourceSkybox::Draw() {
	glDepthMask(GL_FALSE); // superfluous
	glDepthFunc(GL_LEQUAL);
	glUseProgram(programSky);
	float4x4 proj = App->camera->GetProjectionMatrix();
	float4x4 view = App->camera->GetViewMatrix();

	glUniform1i(glGetUniformLocation(programSky, "Skybox"), 0);
	//glUniformMatrix4fv(glGetUniformLocation(programSky, "model"), 1, GL_TRUE, &identityModel[0][0]); //GL_TRUE transpose the matrix
	glUniformMatrix4fv(glGetUniformLocation(programSky, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "proj"), 1, GL_TRUE, &proj[0][0]);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glCubeMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	glDepthMask(GL_TRUE); // superfluous
}