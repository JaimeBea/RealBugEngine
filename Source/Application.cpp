#include "Application.h"
#include "ModuleConfig.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "SDL_timer.h"
#include <windows.h>

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(config = new ModuleConfig());

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
	{
		ret = (*it)->PreInit();
	}

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->PostInit();
	}

	return ret;
}

UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::CONTINUE;

	unsigned now_time = SDL_GetTicks();
	if (SDL_TICKS_PASSED(now_time, previous_time))
	{
		unsigned delta_ms = now_time - previous_time;
		log_delta_ms(delta_ms);
		delta_time = delta_ms / 1000.0f;

		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::CONTINUE; ++it)
		{
			ret = (*it)->PreUpdate();
		}

		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::CONTINUE; ++it)
		{
			ret = (*it)->Update();
		}

		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::CONTINUE; ++it)
		{
			ret = (*it)->PostUpdate();
		}
	}
	previous_time = now_time;

	if (config->limit_framerate)
	{
		now_time = SDL_GetTicks();
		unsigned frame_ms = now_time - previous_time;
		unsigned min_ms = 1000 / config->max_fps;
		if (frame_ms < min_ms)
		{
			SDL_Delay(min_ms - frame_ms);
		}
	}
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;
}

float Application::GetDeltaTime()
{
	return delta_time;
}

void Application::RequestBrowser(char* url)
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
