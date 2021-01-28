#pragma once

#include "Module.h"

#include "Math/float2.h"
#include "SDL_scancode.h"

#define NUM_MOUSE_BUTTONS 5

enum KeyState {
	KS_IDLE = 0,
	KS_DOWN,
	KS_REPEAT,
	KS_UP
};

class ModuleInput : public Module {
public:
	bool Init() override;
	UpdateStatus PreUpdate() override;
	bool CleanUp() override;

	void ReleaseDroppedFilePath();
	void WarpMouse(int mouseX, int mouseY);

	const char* GetDroppedFilePath() const;
	KeyState GetKey(int scancode) const;
	KeyState GetMouseButton(int button) const;
	float GetMouseWheelMotion() const;
	const float2& GetMouseMotion() const;
	const float2& GetMousePosition() const;

private:
	char* droppedFilePath = nullptr;
	KeyState keyboard[SDL_NUM_SCANCODES] = {KS_IDLE};
	KeyState mouseButtons[NUM_MOUSE_BUTTONS] = {KS_IDLE};
	float mouseWheelMotion = 0;
	float2 mouseMotion = {0, 0};
	float2 mouse = {0, 0};
	bool mouseWarped = false;
};