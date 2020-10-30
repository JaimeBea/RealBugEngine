#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/vec2d.h"
#include "SDL_scancode.h"

#define NUM_KEYS 300
#define NUM_MOUSE_BUTTONS 5

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status Update();
	bool CleanUp();

	KeyState GetKey(int scancode) const
	{
		return keyboard[scancode];
	}

	KeyState GetMouseButtonDown(int button) const
	{
		return mouse_buttons[button - 1];
	}

	const float2& GetMouseMotion() const
	{
		return mouse_motion;
	}

	const float2& GetMousePosition() const
	{
		return mouse;
	}

private:
	KeyState keyboard[NUM_KEYS];
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	float2 mouse_motion;
	float2 mouse;
};