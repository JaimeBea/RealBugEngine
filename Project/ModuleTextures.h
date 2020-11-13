#pragma once

#include "Module.h"

#include <vector>

enum class TextureFilter;
enum class TextureWrap;

class ModuleTextures : public Module
{
public:
	bool Init() override;
	bool CleanUp() override;

	unsigned LoadTexture(const char* file_name);
	void ReleaseTexture(unsigned texture);

	void SetMinFilter(TextureFilter filter);
	void SetMagFilter(TextureFilter filter);
	void SetWrap(TextureWrap wrap);

public:
	std::vector<unsigned> textures;
};

