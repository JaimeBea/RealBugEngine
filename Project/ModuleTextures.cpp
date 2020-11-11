#include "ModuleTextures.h"

#include "Globals.h"
#include "Logging.h"

#include "IL/il.h"
#include "GL/glew.h"

ModuleTextures::ModuleTextures() {}

ModuleTextures::~ModuleTextures() {}

bool ModuleTextures::PostInit()
{
	ilInit();

	unsigned image;
	ilGenImages(1, &image);
	DEFER{ ilDeleteImages(1, &image); };

	ilBindImage(image);
	bool success = ilLoadImage("Lenna.png");
	if (!success)
	{
		LOG("Failed to load image.");
		return false;
	}
	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	if (!success)
	{
		LOG("Failed to convert image.");
		return false;
	}

	glGenTextures(1, &lenna); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, lenna); /* Binding of texture name */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
	  interpolation for magnification filter */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
	  interpolation for minifying filter */
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		ilGetData()); /* Texture specification */

	return true;
}

bool ModuleTextures::CleanUp()
{
	glDeleteTextures(1, &lenna);
	return true;
}
