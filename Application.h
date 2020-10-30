#pragma once

#include <list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleCamera;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDeltaTime();

public:
	ModuleRender* renderer = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;

private:
	float delta_time = 0.0f;
	Uint32 previous_time = 0;

	std::list<Module*> modules;

};

extern Application* App;
