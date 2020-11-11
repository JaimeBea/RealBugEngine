#pragma once

#include "Module.h"
#include "Globals.h"

#include "ShortString.h"

struct SDL_version;

class ModuleConfig : public Module
{
public:
	ModuleConfig();
	~ModuleConfig();

	bool PostInit() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

public:
	// Configuration

	ShortString<20> app_name = "Real Engine";
	ShortString<20> organization = "";
	int max_fps = 60;
	bool limit_framerate = true;
	bool vsync = true;
	WindowMode window_mode = WindowMode::WINDOWED;
	bool resizable = true;
	int screen_width = 640;
	int screen_height = 480;
	float brightness = 1.0f;
	TextureFilter min_filter = TextureFilter::NEAREST_MIPMAP_LINEAR;
	TextureFilter mag_filter = TextureFilter::LINEAR;
	TextureWrap texture_wrap = TextureWrap::REPEAT;

	// Information

	const char* engine_name = "Real Engine";
	const char* engine_description = "Simple AAA game engine";
	const char* engine_authors = "Jaime Bea";
	const char* engine_libraries = "SDL2, MathGeoLib, Dear ImGUI";
	const char* engine_license = "MIT";

	ShortString<20> sdl_version = "Not available";
	int cpu_count = 0;
	int cache_size_kb = 0;
	float ram_gb = 0;
	bool caps[14] = { false };
	const char* gpu_vendor = "Not available";
	const char* gpu_renderer = "Not available";
	const char* gpu_opengl_version = "Not available";
	float vram_budget_mb = 0;
	float vram_available_mb = 0;
	float vram_usage_mb = 0;
	float vram_reserved_mb = 0;
};


