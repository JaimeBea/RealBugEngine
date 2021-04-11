#include "ModuleInput.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleUserInterface.h"
#include "Utils/Logging.h"

#include "imgui_impl_sdl.h"
#include "SDL.h"
#include "Brofiler.h"

#include "Utils/Leaks.h"

bool ModuleInput::Init() {
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

UpdateStatus ModuleInput::PreUpdate() {
	BROFILER_CATEGORY("ModuleInput - PreUpdate", Profiler::Color::AntiqueWhite)

	ImGuiIO& io = ImGui::GetIO();

	mouseMotion = {0, 0};
	mouseWheelMotion = 0;

	int windowId = SDL_GetWindowID(App->window->window);

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
		if (keyboard[i] == KS_DOWN) {
			keyboard[i] = KS_REPEAT;
		}

		if (keyboard[i] == KS_UP) {
			keyboard[i] = KS_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i) {
		if (mouseButtons[i] == KS_DOWN) {
			mouseButtons[i] = KS_REPEAT;
		}

		if (mouseButtons[i] == KS_UP) {
			mouseButtons[i] = KS_IDLE;
		}
	}

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type) {
		case SDL_QUIT:
			return UpdateStatus::STOP;

		case SDL_WINDOWEVENT:
			if (event.window.windowID == windowId) {
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					return UpdateStatus::STOP;
				}
			}
			break;

		case SDL_DROPFILE:
			ReleaseDroppedFilePath();
			droppedFilePath = event.drop.file;
			LOG("File dropped: %s", droppedFilePath);
			break;

		case SDL_MOUSEWHEEL:
			if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
				mouseWheelMotion = (float) event.wheel.x;
			} else {
				mouseWheelMotion = (float) event.wheel.y;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouseButtons[event.button.button - 1] = KS_DOWN;
			if (event.button.button == SDL_BUTTON_LEFT) {
				App->editor->OnMouseClicked();
			}
			break;
		case SDL_MOUSEBUTTONUP:
			mouseButtons[event.button.button - 1] = KS_UP;
			break;

		case SDL_KEYDOWN:
			keyboard[event.key.keysym.scancode] = KS_DOWN;
			break;

		case SDL_KEYUP:
			keyboard[event.key.keysym.scancode] = KS_UP;
			break;
		case SDL_MOUSEMOTION:
			//IF EDITOR DO THIS
			App->editor->OnMouseMoved();
			//IF NOT EDITOR USE ACTUAL MOUSE POS
			break;
		}
	}

	if (io.WantCaptureKeyboard) {
		for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
			keyboard[i] = KS_IDLE;
		}
	}

	if (io.WantCaptureMouse) {
		mouseWheelMotion = 0;
		mouseMotion.x = 0;
		mouseMotion.y = 0;

		for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i) {
			mouseButtons[i] = KS_IDLE;
		}
	} else {
		int mouseX;
		int mouseY;
		SDL_GetGlobalMouseState(&mouseX, &mouseY);
		if (!mouseWarped) {
			mouseMotion.x = mouseX - mouse.x;
			mouseMotion.y = mouseY - mouse.y;
		} else {
			mouseWarped = false;
		}
		mouse.x = (float) mouseX;
		mouse.y = (float) mouseY;
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleInput::CleanUp() {
	ReleaseDroppedFilePath();
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::ReleaseDroppedFilePath() {
	if (droppedFilePath != nullptr) {
		SDL_free(droppedFilePath);
		droppedFilePath = nullptr;
	}
}

void ModuleInput::WarpMouse(int mouseX, int mouseY) {
	SDL_WarpMouseGlobal(mouseX, mouseY);
	mouseWarped = true;
}

const char* ModuleInput::GetDroppedFilePath() const {
	return droppedFilePath;
}

KeyState ModuleInput::GetKey(int scancode) const {
	return keyboard[scancode];
}

KeyState ModuleInput::GetMouseButton(int button) const {
	return mouseButtons[button - 1];
}

float ModuleInput::GetMouseWheelMotion() const {
	return mouseWheelMotion;
}

const float2& ModuleInput::GetMouseMotion() const {
	return mouseMotion;
}

const float2& ModuleInput::GetMousePosition() const {
	return mouse;
}

KeyState* ModuleInput::GetMouseButtons() {
	return mouseButtons;
}

KeyState* ModuleInput::GetKeyboard() {
	return keyboard;
}