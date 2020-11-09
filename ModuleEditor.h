#pragma once

#include "Module.h"
#include "imgui_impl_sdl.h" // This is neccessary for some reason. Putting it in the .cpp file makes the linker fail

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	virtual ~ModuleEditor();

	bool Init();
	bool PostInit();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	bool show_console = true;
	bool show_config = true;
	bool show_about = false;
};

