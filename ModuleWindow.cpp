#include "ModuleWindow.h"
#include "SDL.h"

ModuleWindow::ModuleWindow() {}

ModuleWindow::~ModuleWindow() {}

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

		int width = SCREEN_WIDTH;
		int height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if (RESIZABLE)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		switch (SCREEN_MODE)
		{
		case SM_BORDERLESS:
			flags |= SDL_WINDOW_BORDERLESS;
			break;
		case SM_FULLSCREEN:
			flags |= SDL_WINDOW_FULLSCREEN;
			break;
		case SM_FULLSCREEN_DESKTOP:
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			break;
		}

		window = SDL_CreateWindow(APP_NAME.GetChars(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			screen_surface = SDL_GetWindowSurface(window);
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

void ModuleWindow::SetScreenMode(int mode)
{
	switch (mode)
	{
	case SM_WINDOWED:
		SDL_SetWindowFullscreen(window, SDL_FALSE);
		SDL_SetWindowBordered(window, SDL_TRUE);
		break;
	case SM_BORDERLESS:
		SDL_SetWindowFullscreen(window, SDL_FALSE);
		SDL_SetWindowBordered(window, SDL_FALSE);
		break;
	case SM_FULLSCREEN:
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		break;
	case SM_FULLSCREEN_DESKTOP:
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
