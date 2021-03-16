#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "FileSystem/MeshImporter.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/AnimationImporter.h"
#include "Modules/ModuleFiles.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <string>

#include "Utils/Leaks.h"

bool ModuleResources::Init() {
	textures.Allocate(100);
	cubeMaps.Allocate(10);
	meshes.Allocate(1000);
	animations.Allocate(10);
	animationControllers.Allocate(10);

	ilInit();
	iluInit();

	return true;
}

bool ModuleResources::CleanUp() {
	ReleaseAll();

	return true;
}

Texture* ModuleResources::ObtainTexture() {
	return textures.Obtain();
}

void ModuleResources::ReleaseTexture(Texture* texture) {
	TextureImporter::UnloadTexture(texture);
	textures.Release(texture);
}

CubeMap* ModuleResources::ObtainCubeMap() {
	return cubeMaps.Obtain();
}

void ModuleResources::ReleaseCubeMap(CubeMap* cubeMap) {
	TextureImporter::UnloadCubeMap(cubeMap);
	cubeMaps.Release(cubeMap);
}

Mesh* ModuleResources::ObtainMesh() {
	return meshes.Obtain();
}

void ModuleResources::ReleaseMesh(Mesh* mesh) {
	MeshImporter::UnloadMesh(mesh);
	meshes.Release(mesh);
}

ResourceAnimation* ModuleResources::ObtainAnimation() {
	return animations.Obtain();
}

void ModuleResources::ReleaseAnimation(ResourceAnimation* animation) {
	animations.Release(animation);
}

AnimationController* ModuleResources::ObtainAnimationController() {
	return animationControllers.Obtain();
}

void ModuleResources::ReleaseAnimationController(AnimationController* animationController) {
	animationControllers.Release(animationController);
}

void ModuleResources::ReleaseAll() {
	for (Texture& texture : textures) {
		ReleaseTexture(&texture);
	}

	for (CubeMap& cubeMap : cubeMaps) {
		ReleaseCubeMap(&cubeMap);
	}

	for (Mesh& mesh : meshes) {
		ReleaseMesh(&mesh);
	}

	for (ResourceAnimation& animation : animations) {
		ReleaseAnimation(&animation);
	}

	for (AnimationController& animationController : animationControllers) {
		ReleaseAnimationController(&animationController);
	}
}

void ModuleResources::SetMinFilter(TextureMinFilter filter) {
	for (Texture& texture : textures) {
		glBindTexture(GL_TEXTURE_2D, texture.glTexture);
		switch (filter) {
		case TextureMinFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case TextureMinFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	}

	minFilter = filter;
}

void ModuleResources::SetMagFilter(TextureMagFilter filter) {
	for (Texture& texture : textures) {
		glBindTexture(GL_TEXTURE_2D, texture.glTexture);
		switch (filter) {
		case TextureMagFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureMagFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	magFilter = filter;
}

void ModuleResources::SetWrap(TextureWrap wrap) {
	for (Texture& texture : textures) {
		glBindTexture(GL_TEXTURE_2D, texture.glTexture);
		switch (wrap) {
		case TextureWrap::REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		case TextureWrap::CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case TextureWrap::CLAMP_TO_BORDER:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			break;
		case TextureWrap::MIRROR_REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			break;
		case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
			break;
		}
	}

	textureWrap = wrap;
}

TextureMinFilter ModuleResources::GetMinFilter() const {
	return minFilter;
}

TextureMagFilter ModuleResources::GetMagFilter() const {
	return magFilter;
}

TextureWrap ModuleResources::GetWrap() const {
	return textureWrap;
}
