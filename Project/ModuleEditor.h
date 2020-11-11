#pragma once

#include "Module.h"

class ModuleEditor : public Module
{
public:
	bool Init() override;
	bool PostInit() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

private:
	bool show_console = true;
	bool show_config = true;
	bool show_about = false;
};

