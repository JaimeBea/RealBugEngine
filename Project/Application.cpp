#include "Application.h"

#include "Globals.h"
#include "Logging.h"
#include "ModuleHardwareInfo.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModulePrograms.h"
#include "ModuleDebugDraw.h"
#include "ModuleTextures.h"
#include "ModuleModels.h"

#include "SDL_timer.h"
#include <windows.h>

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(hardware = new ModuleHardwareInfo());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(models = new ModuleModels());
	modules.push_back(programs = new ModulePrograms());

	modules.push_back(input = new ModuleInput());

	modules.push_back(editor = new ModuleEditor());
	modules.push_back(debug_draw = new ModuleDebugDraw());
	modules.push_back(camera = new ModuleCamera());

	modules.push_back(renderer = new ModuleRender());
}

Application::~Application()
{
	for(Module* module : modules)
    {
        delete module;
    }
}

bool Application::Init()
{
	timer.Start();

	bool ret = true;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Start();
	}

	return ret;
}

UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::CONTINUE;

	unsigned int delta_ms = timer.Read();
	timer.Start();

	if (delta_ms > 0)
	{
		log_delta_ms((float)delta_ms);
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

	if (limit_framerate)
	{
		unsigned int frame_ms = timer.Read();
		unsigned int min_ms = 1000 / max_fps;
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
