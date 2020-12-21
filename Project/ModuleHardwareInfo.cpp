#include "ModuleHardwareInfo.h"

#include "Globals.h"
#include "Application.h"

#include "SDL_version.h"
#include "SDL_cpuinfo.h"
#include "SDL_video.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "assimp/version.h"
#include "Brofiler.h"

#include "Leaks.h"

bool ModuleHardwareInfo::Start()
{
	SDL_version sdl_version_struct;
	SDL_VERSION(&sdl_version_struct);

	sprintf_s(glew_version, "%i.%i.%i", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
	sprintf_s(sdl_version, "%i.%i.%i", sdl_version_struct.major, sdl_version_struct.minor, sdl_version_struct.patch);
	sprintf_s(assimp_version, "%i.%i.%i", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
	sprintf_s(devil_version, "%i.%i.%i", IL_VERSION / 100, (IL_VERSION % 100) / 10, IL_VERSION % 10);

	cpu_count = SDL_GetCPUCount();
	cache_size_kb = SDL_GetCPUCacheLineSize();
	ram_gb = SDL_GetSystemRAM() / 1000.0f;
	caps[0] = SDL_Has3DNow();
	caps[1] = SDL_HasARMSIMD();
	caps[2] = SDL_HasAVX();
	caps[3] = SDL_HasAVX2();
	caps[4] = SDL_HasAVX512F();
	caps[5] = SDL_HasAltiVec();
	caps[6] = SDL_HasMMX();
	caps[7] = SDL_HasNEON();
	caps[8] = SDL_HasRDTSC();
	caps[9] = SDL_HasSSE();
	caps[10] = SDL_HasSSE2();
	caps[11] = SDL_HasSSE3();
	caps[12] = SDL_HasSSE41();
	caps[13] = SDL_HasSSE42();

	gpu_vendor = (const char*) glGetString(GL_VENDOR);
	gpu_renderer = (const char*) glGetString(GL_RENDERER);
	gpu_opengl_version = (const char*) glGetString(GL_VERSION);

	return true;
}

UpdateStatus ModuleHardwareInfo::Update()
{
	BROFILER_CATEGORY("ModuleHardwareInfo - PreUpdate", Profiler::Color::Orange)

	int vram_budget_kb;
	int vram_available_kb;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget_kb);
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_available_kb);
	vram_budget_mb = vram_budget_kb / 1000.0f;
	vram_available_mb = vram_available_kb / 1000.0f;
	vram_usage_mb = vram_budget_mb - vram_available_mb;

	return UpdateStatus::CONTINUE;
}

bool ModuleHardwareInfo::CleanUp()
{
	return true;
}
