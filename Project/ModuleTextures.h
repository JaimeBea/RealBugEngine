#pragma once

#include "Module.h"

enum class TextureFilter;
enum class TextureWrap;

class ModuleTextures : public Module
{
public:
	bool PostInit() override;
	bool CleanUp() override;

	void SetMinFilter(TextureFilter filter);
	void SetMagFilter(TextureFilter filter);

	void SetWrap(TextureWrap wrap);

public:
	unsigned lenna = 0;
};

