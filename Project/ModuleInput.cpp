#include "ModuleInput.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_sdl.h"
#include "SDL.h"

bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

UpdateStatus ModuleInput::PreUpdate()
{
	ImGuiIO& io = ImGui::GetIO();

	mouse_motion = { 0, 0 };
	mouse_wheel_motion = 0;

	int window_id = SDL_GetWindowID(App->window->window);

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		if (keyboard[i] == KS_DOWN)
		{
			keyboard[i] = KS_REPEAT;
		}

		if (keyboard[i] == KS_UP)
		{
			keyboard[i] = KS_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KS_DOWN)
		{
			mouse_buttons[i] = KS_REPEAT;
		}

		if (mouse_buttons[i] == KS_UP)
		{
			mouse_buttons[i] = KS_IDLE;
		}
	}

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		
		switch (event.type)
		{
		case SDL_QUIT:
			return UpdateStatus::STOP;

		case SDL_WINDOWEVENT:
			if (event.window.windowID == window_id)
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					return UpdateStatus::STOP;
				}
			}
			break;

		case SDL_DROPFILE:
			ReleaseDroppedFileName();
			dropped_file_name = event.drop.file;
			LOG("File dropped: %s", dropped_file_name);
			break;

		case SDL_MOUSEWHEEL:
			if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			{
				mouse_wheel_motion = (float) event.wheel.x;
			}
			else
			{
				mouse_wheel_motion = (float) event.wheel.y;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[event.button.button - 1] = KS_DOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[event.button.button - 1] = KS_UP;
			break;

		case SDL_KEYDOWN:
			keyboard[event.key.keysym.scancode] = KS_DOWN;
			break;

		case SDL_KEYUP:
			keyboard[event.key.keysym.scancode] = KS_UP;
			break;
		}
	}

	if (io.WantCaptureKeyboard)
	{
		for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
		{
			keyboard[i] = KS_IDLE;
		}
	}

	if (io.WantCaptureMouse)
	{
		mouse_wheel_motion = 0;
		mouse_motion.x = 0;
		mouse_motion.y = 0;

		for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
		{
			mouse_buttons[i] = KS_IDLE;
		}
	}
	else
	{
		int mouse_x;
		int mouse_y;
		SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
		if (!mouse_warped)
		{
			mouse_motion.x = mouse_x - mouse.x;
			mouse_motion.y = mouse_y - mouse.y;
		}
		else
		{
			mouse_warped = false;
		}
		mouse.x = (float)mouse_x;
		mouse.y = (float)mouse_y;
	}

    return UpdateStatus::CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	ReleaseDroppedFileName();
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::ReleaseDroppedFileName()
{
	if (dropped_file_name != nullptr)
	{
		SDL_free(dropped_file_name);
		dropped_file_name = nullptr;
	}
}

void ModuleInput::WarpMouse(int mouse_x, int mouse_y)
{
	SDL_WarpMouseGlobal(mouse_x, mouse_y);
	mouse_warped = true;
}
