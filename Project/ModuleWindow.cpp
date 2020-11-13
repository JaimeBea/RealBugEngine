#include "ModuleWindow.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleConfig.h"

#include "SDL.h"

bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		// Create window

		int width = App->config->screen_width;
		int height = App->config->screen_height;
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if (App->config->resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		switch (App->config->window_mode)
		{
		case WindowMode::BORDERLESS:
			flags |= SDL_WINDOW_BORDERLESS;
			break;
		case WindowMode::FULLSCREEN:
			flags |= SDL_WINDOW_FULLSCREEN;
			break;
		case WindowMode::FULLSCREEN_DESKTOP:
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			break;
		}

		window = SDL_CreateWindow(App->config->app_name.GetChars(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	if(window != NULL)
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
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		break;
	case WindowMode::FULLSCREEN_DESKTOP:
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	}
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

void ModuleWindow::SetSize(int width, int height)
{
	SDL_SetWindowSize(window, width, height);
}

void ModuleWindow::SetBrightness(float brightness)
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::SetTitle(const char* title)
{

}
