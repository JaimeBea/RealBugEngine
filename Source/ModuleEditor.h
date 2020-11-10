#pragma once

#include "Module.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	virtual ~ModuleEditor();

	bool Init();
	bool PostInit();
	UpdateStatus PreUpdate();
	UpdateStatus Update();
	UpdateStatus PostUpdate();
	bool CleanUp();

private:
	bool show_console = true;
	bool show_config = true;
	bool show_about = false;
};

