#include "ModuleTextures.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleConfig.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

bool ModuleTextures::Init()
{
	ilInit();
	iluInit();

	return true;
}

bool ModuleTextures::CleanUp()
{
	glDeleteTextures(textures.size(), textures.data());

	return true;
}

unsigned ModuleTextures::LoadTexture(const char* file_name)
{
	LOG("Loading texture from path: \"%s\".", file_name);

	unsigned image;
	ilGenImages(1, &image);
	DEFER{ ilDeleteImages(1, &image); };

	ilBindImage(image);
	bool image_loaded = ilLoadImage(file_name);
	if (!image_loaded)
	{
		LOG("Failed to load image.");
		return 0;
	}
	bool image_converted = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	if (!image_converted)
	{
		LOG("Failed to convert image.");
		return 0;
	}

	unsigned texture;
	glGenTextures(1, &texture); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, texture); /* Binding of texture name */

	SetWrap(App->config->texture_wrap);
	SetMinFilter(App->config->min_filter);
	SetMagFilter(App->config->mag_filter);

	// Flip image if neccessary
	ILinfo info;
	iluGetImageInfo(&info);
	if (info.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	// Generate texture from image
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		ilGetData()); /* Texture specification */

	glGenerateMipmap(GL_TEXTURE_2D);

	textures.push_back(texture);

	LOG("Texture loaded successfuly.");

	return texture;
}

void ModuleTextures::ReleaseTexture(unsigned texture)
{
	bool found = false;
	for (std::vector<unsigned>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (*it == texture)
		{
			textures.erase(it);
			found = true;
			break;
		}
	}

	if (!found)
	{
		LOG("Tried to release non-existent texture (%i).", texture);
		return;
	}

	glDeleteTextures(1, &texture);
}

void ModuleTextures::SetMinFilter(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case TextureFilter::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case TextureFilter::NEAREST_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case TextureFilter::LINEAR_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case TextureFilter::NEAREST_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case TextureFilter::LINEAR_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	}
}

void ModuleTextures::SetMagFilter(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case TextureFilter::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFilter::NEAREST_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case TextureFilter::LINEAR_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case TextureFilter::NEAREST_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case TextureFilter::LINEAR_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	}
}

void ModuleTextures::SetWrap(TextureWrap wrap)
{
	switch (wrap)
	{
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
