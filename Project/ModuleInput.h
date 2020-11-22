#pragma once

#include "Module.h"

#include "Math/float2.h"
#include "SDL_scancode.h"

#define NUM_MOUSE_BUTTONS 5

enum KeyState
{
	KS_IDLE = 0,
	KS_DOWN,
	KS_REPEAT,
	KS_UP
};

class ModuleInput : public Module
{
public:
	bool Init() override;
	UpdateStatus PreUpdate() override;
	bool CleanUp() override;

	void ReleaseDroppedFileName();

	const char* GetDroppedFileName() const
	{
		return dropped_file_name;
	}

	KeyState GetKey(int scancode) const
	{
		return keyboard[scancode];
	}

	KeyState GetMouseButton(int button) const
	{
		return mouse_buttons[button - 1];
	}

	float GetMouseWheelMotion() const
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
	char* dropped_file_name = nullptr;
	KeyState keyboard[SDL_NUM_SCANCODES] = { KS_IDLE };
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS] = { KS_IDLE };
	float mouse_wheel_motion = 0;
	float2 mouse_motion = { 0, 0 };
	float2 mouse = { 0, 0 };
};