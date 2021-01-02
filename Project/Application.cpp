#include "Application.h"

#include "Globals.h"
#include "Logging.h"
#include "ModuleHardwareInfo.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleSceneRender.h"
#include "ModuleRender.h"
#include "ModulePrograms.h"
#include "ModuleDebugDraw.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "SDL_timer.h"
#include <windows.h>
#include "Brofiler.h"

#include "Leaks.h"

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(hardware = new ModuleHardwareInfo());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(programs = new ModulePrograms());

	modules.push_back(time = new ModuleTime());
	modules.push_back(input = new ModuleInput());

	modules.push_back(scene = new ModuleScene());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(debug_draw = new ModuleDebugDraw());
	modules.push_back(camera = new ModuleCamera());

	modules.push_back(scene_renderer = new ModuleSceneRender());
	modules.push_back(renderer = new ModuleRender());
}

Application::~Application()
{
	for (Module* module : modules)
	{
		delete module;
	}
}

bool Application::Init()
{
	bool ret = true;

	for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Start();
	}

	return ret;
}

UpdateStatus Application::Update()
{
	BROFILER_CATEGORY("App - Update", Profiler::Color::Red)

	UpdateStatus ret = UpdateStatus::CONTINUE;

	for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::CONTINUE; ++it)
	{
		ret = (*it)->PreUpdate();
	}

	for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::CONTINUE; ++it)
	{
		ret = (*it)->Update();
	}

	for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus::CONTINUE; ++it)
	{
		ret = (*it)->PostUpdate();
	}

	time->WaitForEndOfFrame();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;
}

void Application::RequestBrowser(char* url)
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
