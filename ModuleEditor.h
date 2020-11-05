#pragma once

#include "Module.h"
#include "imgui_impl_sdl.h" // This is neccessary for some reason. Putting it in the .cpp file makes the linker fail

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	virtual ~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
};

