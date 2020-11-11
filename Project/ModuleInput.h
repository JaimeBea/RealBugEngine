#pragma once

#include "Module.h"
#include "Math/float2.h"
#include "SDL_scancode.h"

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

	bool Init() override;
	UpdateStatus PreUpdate() override;
	bool CleanUp() override;

	KeyState GetKey(int scancode) const
	{
		return keyboard[scancode];
	}

	KeyState GetMouseButton(int button) const
	{
		return mouse_buttons[button - 1];
	}

	const float GetMouseWheelMotion() const
	{
		return mouse_wheel_motion;
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
	KeyState keyboard[SDL_NUM_SCANCODES];
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	float mouse_wheel_motion = 0;
	float2 mouse_motion = { 0, 0 };
	float2 mouse = { 0, 0 };
};