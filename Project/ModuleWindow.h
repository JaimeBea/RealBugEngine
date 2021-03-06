#pragma once

#include "Module.h"

#include "SDL_video.h"
#include <vector>

struct SDL_Surface;

enum class WindowMode
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
	FULLSCREEN_DESKTOP
};

class ModuleWindow : public Module
{
public:
	bool Init() override;
	bool CleanUp() override;

	void SetWindowMode(WindowMode mode);
	void SetResizable(bool resizable);
	void SetCurrentDisplayMode(int index);
	void SetSize(int width, int height);
	void SetBrightness(float brightness);
	void SetTitle(const char* title);

	WindowMode GetWindowMode() const;
	bool GetMaximized() const;
	bool GetResizable() const;
	int GetCurrentDisplayMode() const;
	int GetWidth() const;
	int GetHeight() const;
	float GetBrightness() const;
	const char* GetTitle() const;

public:
	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;

	std::vector<SDL_DisplayMode> display_modes;

	int default_window_width = 1280;
	int default_window_height = 720;

private:
	WindowMode window_mode = WindowMode::WINDOWED;
	int current_display_mode = 0;
};
