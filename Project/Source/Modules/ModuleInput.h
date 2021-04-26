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
	// ------- Core Functions ------ //
	bool Init() override;
	UpdateStatus PreUpdate() override; // All button and keys pressed are processed here.
	bool CleanUp() override;

	void ReleaseDroppedFilePath();			// Calls a SDL_free() to release 'droppedFilePath'.
	void WarpMouse(int mouseX, int mouseY); // Places the mouse in a given X,Y position in the screen. Called from ModuleCamera when rotating or orbiting, to allow rotating infinitely.

	// ---------- Getters ---------- //
	const char* GetDroppedFilePath() const;
	KeyState GetKey(int scancode) const;
	KeyState GetMouseButton(int button) const;
	float GetMouseWheelMotion() const;
	const float2& GetMouseMotion() const;
	const float2& GetMousePosition(bool sceneSensitive = false) const;
	KeyState* GetMouseButtons();
	KeyState* GetKeyboard();

private:
	char* droppedFilePath = nullptr;					  // SDL_DropEvent. Stores the path of a file when it is drag&dropped into the engine.
	KeyState keyboard[SDL_NUM_SCANCODES] = {KS_IDLE};	  // Array that stores the 'KeyState' of every key in the keyboard. See KeyState for possible states.
	KeyState mouseButtons[NUM_MOUSE_BUTTONS] = {KS_IDLE}; // Same keystate, but for the mouse buttons.
	float mouseWheelMotion = 0;							  // Stores the increment registered by the mouse wheel on a frame.
	float2 mouseMotion = {0, 0};						  // Stores de movement increment of the mouse position on a frame.
	float2 mouse = {0, 0};								  // Stores the mouse position.
	bool mouseWarped = false;							  // "Flag" to indicate that the mouse has been warped. When warping happens and it is set to true, the PreUpdate() will ignore the mouse motion for one frame.
};