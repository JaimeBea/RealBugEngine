#pragma once

#include <list>

enum class UpdateStatus;

class Module;
class ModuleConfig;
class ModuleRender;
class ModuleEditor;
class ModuleCamera;
class ModuleWindow;
class ModuleTextures;
class ModuleModels;
class ModuleInput;
class ModuleProgram;
class ModuleDebugDraw;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	UpdateStatus Update();
	bool CleanUp();

	float GetDeltaTime();
	void RequestBrowser(char* url);

public:
	ModuleConfig* config = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleModels* models = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleProgram* program = nullptr;
	ModuleDebugDraw* debug_draw = nullptr;

private:
	float delta_time = 0.0f;
	unsigned previous_time = 0;

	std::list<Module*> modules;
};

extern Application* App;
