#include "PerformanceTimer.h"

#include "Globals.h"

#include "SDL_timer.h"

#include "Utils/Leaks.h"

void PerformanceTimer::Start()
{
	start_count = SDL_GetPerformanceCounter();
	delta_time = 0;
	running = true;
}

unsigned long long PerformanceTimer::Stop()
{
	if (running)
	{
		delta_time = (SDL_GetPerformanceCounter() - start_count) * 1000000 / SDL_GetPerformanceFrequency();
		running = false;
	}

	return delta_time;
}

unsigned long long PerformanceTimer::Read()
{
	if (running)
	{
		delta_time = (SDL_GetPerformanceCounter() - start_count) * 1000000 / SDL_GetPerformanceFrequency();
	}

	return delta_time;
}
