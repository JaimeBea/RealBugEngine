#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "FileSystem/MeshImporter.h"
#include "FileSystem/TextureImporter.h"
#include "Modules/ModuleFiles.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <string>

#include "Utils/Leaks.h"

bool ModuleResources::Init() {
	textures.Allocate(100);
	cube_maps.Allocate(10);
	meshes.Allocate(1000);

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
	return cube_maps.Obtain();
}

void ModuleResources::ReleaseCubeMap(CubeMap* cube_map) {
	TextureImporter::UnloadCubeMap(cube_map);
	cube_maps.Release(cube_map);
}

Mesh* ModuleResources::ObtainMesh() {
	return meshes.Obtain();
}

void ModuleResources::ReleaseMesh(Mesh* mesh) {
	MeshImporter::UnloadMesh(mesh);
	meshes.Release(mesh);
}

void ModuleResources::ReleaseAll() {
	for (Texture& texture : textures) {
		ReleaseTexture(&texture);
	}

	for (CubeMap& cube_map : cube_maps) {
		ReleaseCubeMap(&cube_map);
	}

	for (Mesh& mesh : meshes) {
		ReleaseMesh(&mesh);
	}
}

void ModuleResources::SetMinFilter(TextureMinFilter filter) {
	for (Texture& texture : textures) {
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
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

	min_filter = filter;
}

void ModuleResources::SetMagFilter(TextureMagFilter filter) {
	for (Texture& texture : textures) {
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter) {
		case TextureMagFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureMagFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	mag_filter = filter;
}

void ModuleResources::SetWrap(TextureWrap wrap) {
	for (Texture& texture : textures) {
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
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

	texture_wrap = wrap;
}

TextureMinFilter ModuleResources::GetMinFilter() const {
	return min_filter;
}

TextureMagFilter ModuleResources::GetMagFilter() const {
	return mag_filter;
}

TextureWrap ModuleResources::GetWrap() const {
	return texture_wrap;
}
