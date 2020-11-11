#pragma once

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:
	ModuleWindow();
	~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;

	void SetWindowMode(int mode);
	void SetResizable(bool resizable);
	void SetSize(int width, int height);
	void SetBrightness(float brightness);
	void SetTitle(const char* title);

public:
	SDL_Window* window = nullptr; // The window we'll be rendering to
	SDL_Surface* screen_surface = nullptr; // The surface contained by the window
};
