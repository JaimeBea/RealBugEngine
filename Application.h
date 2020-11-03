#pragma once

#include <list>
#include "Globals.h"

class Module;
class ModuleRender;
class ModuleCamera;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleProgram;

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
	ModuleRenderExercise* renderer_exercise = nullptr;
	ModuleProgram* program = nullptr;

private:
	float delta_time = 0.0f;
	unsigned previous_time = 0;

	std::list<Module*> modules;

};

extern Application* App;
