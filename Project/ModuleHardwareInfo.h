#pragma once

#include "Module.h"
#include "Globals.h"

struct SDL_version;

class ModuleHardwareInfo : public Module
{
public:
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

public:
	char sdl_version[20] = "Not available";
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


