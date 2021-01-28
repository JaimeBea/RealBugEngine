#include "PerformanceTimer.h"

#include "Globals.h"

#include "SDL_timer.h"

#include "Utils/Leaks.h"

void PerformanceTimer::Start() {
	startCount = SDL_GetPerformanceCounter();
	deltaTime = 0;
	running = true;
}

unsigned long long PerformanceTimer::Stop() {
	if (running) {
		deltaTime = (SDL_GetPerformanceCounter() - startCount) * 1000000 / SDL_GetPerformanceFrequency();
		running = false;
	}

	return deltaTime;
}

unsigned long long PerformanceTimer::Read() {
	if (running) {
		deltaTime = (SDL_GetPerformanceCounter() - startCount) * 1000000 / SDL_GetPerformanceFrequency();
	}

	return deltaTime;
}
