#pragma once

#include <list>

enum class UpdateStatus;

class Module;
class ModuleHardwareInfo;
class ModuleRender;
class ModuleEditor;
class ModuleCamera;
class ModuleWindow;
class ModuleTextures;
class ModuleModels;
class ModuleInput;
class ModulePrograms;
class ModuleDebugDraw;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	float GetDeltaTime();
	void RequestBrowser(char* url);

public:
	ModuleHardwareInfo* hardware = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleModels* models = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditor* editor = nullptr;
	ModulePrograms* programs = nullptr;
	ModuleDebugDraw* debug_draw = nullptr;

	// Application Configuration

	char app_name[20] = "Real Engine";
	char organization[20] = "";
	int max_fps = 60;
	bool limit_framerate = true;
	bool vsync = true;

	// Engine Information

	const char* engine_name = "Real Engine";
	const char* engine_description = "Simple AAA game engine";
	const char* engine_authors = "Jaime Bea";
	const char* engine_libraries = "GLEW, SDL2, MathGeoLib, Dear ImGUI, DeviL, assimp";
	const char* engine_license = "MIT";

private:
	float delta_time = 0.0f;
	unsigned previous_time = 0;

	std::list<Module*> modules;
};

extern Application* App;
