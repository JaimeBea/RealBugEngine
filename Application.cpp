#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "SDL_timer.h"

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());

	modules.push_back(editor = new ModuleEditor());
	modules.push_back(camera = new ModuleCamera());

	modules.push_back(renderer = new ModuleRender());
	//modules.push_back(renderer_exercise = new ModuleRenderExercise());
	//modules.push_back(program = new ModuleProgram());
}

Application::~Application()
{
	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	previous_time = SDL_GetTicks();

	bool ret = true;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->PreInit();

	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->PostInit();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	unsigned now_time = SDL_GetTicks();
	if (SDL_TICKS_PASSED(now_time, previous_time))
	{
		delta_time = (now_time - previous_time) / 1000.0f;

		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->PreUpdate();

		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->Update();

		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->PostUpdate();
	}
	previous_time = now_time;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

float Application::GetDeltaTime()
{
	return delta_time;
}
