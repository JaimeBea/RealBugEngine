#include "ModuleWindow.h"

#include "Globals.h"
#include "Logging.h"
#include "Application.h"

#include "SDL.h"

#include "Leaks.h"

bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

	SDL_DisplayMode desktop_display_mode;
	SDL_GetDesktopDisplayMode(0, &desktop_display_mode);
	window = SDL_CreateWindow(App->app_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, desktop_display_mode.w - 100, desktop_display_mode.h - 100, flags);
	if (window == NULL)
	{
		LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	surface = SDL_GetWindowSurface(window);

	// Create a list with all the available display modes
	int display_mode_num = SDL_GetNumDisplayModes(SDL_GetWindowDisplayIndex(window));
	for (int i = 0; i < display_mode_num; ++i)
	{
		SDL_DisplayMode display_mode;
		SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(window), i, &display_mode);
		display_modes.push_back(display_mode);
	}

	// Set the current display mode to the first one
	SDL_SetWindowDisplayMode(window, &display_modes[current_display_mode]);

	return true;
}

bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	if (window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
	return true;
}

void ModuleWindow::SetWindowMode(WindowMode mode)
{
	switch (mode)
	{
	case WindowMode::WINDOWED:
		SDL_SetWindowFullscreen(window, SDL_FALSE);
		SDL_SetWindowBordered(window, SDL_TRUE);
		break;
	case WindowMode::BORDERLESS:
		SDL_SetWindowFullscreen(window, SDL_FALSE);
		SDL_SetWindowBordered(window, SDL_FALSE);
		break;
	case WindowMode::FULLSCREEN:
		SDL_SetWindowDisplayMode(window, &display_modes[current_display_mode]);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		break;
	case WindowMode::FULLSCREEN_DESKTOP:
		SDL_SetWindowDisplayMode(window, &display_modes[current_display_mode]);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	}
	window_mode = mode;
}

void ModuleWindow::SetResizable(bool resizable)
{
	if (resizable)
	{
		SDL_SetWindowResizable(window, SDL_TRUE);
	}
	else
	{
		SDL_SetWindowResizable(window, SDL_FALSE);
	}
}

void ModuleWindow::SetCurrentDisplayMode(int index)
{
	SDL_SetWindowDisplayMode(window, &display_modes[index]);
	current_display_mode = index;
}

void ModuleWindow::SetSize(int width, int height)
{
	int display_index = SDL_GetWindowDisplayIndex(window);
	SDL_SetWindowSize(window, width, height);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(display_index), SDL_WINDOWPOS_CENTERED_DISPLAY(display_index));
}

void ModuleWindow::SetBrightness(float brightness)
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

WindowMode ModuleWindow::GetWindowMode() const
{
	return window_mode;
}

bool ModuleWindow::GetMaximized() const
{
	return (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
}

bool ModuleWindow::GetResizable() const
{
	return (SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE) != 0;
}

int ModuleWindow::GetCurrentDisplayMode() const
{
	return current_display_mode;
}

int ModuleWindow::GetWidth() const
{
	int width;
	int height;
	SDL_GetWindowSize(window, &width, &height);
	return width;
}

int ModuleWindow::GetHeight() const
{
	int width;
	int height;
	SDL_GetWindowSize(window, &width, &height);
	return height;
}

float ModuleWindow::GetBrightness() const
{
	return SDL_GetWindowBrightness(window);
}

const char* ModuleWindow::GetTitle() const
{
	return SDL_GetWindowTitle(window);
}
