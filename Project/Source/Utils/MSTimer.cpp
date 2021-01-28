#include "MSTimer.h"

#include "Globals.h"

#include "SDL_timer.h"

#include "Utils/Leaks.h"

void MSTimer::Start() {
	startTime = SDL_GetTicks();
	deltaTime = 0;
	running = true;
}

unsigned int MSTimer::Stop() {
	if (running) {
		unsigned int nowTime = SDL_GetTicks();
		if (SDL_TICKS_PASSED(nowTime, startTime)) {
			deltaTime = nowTime - startTime;
		} else {
			deltaTime = 0;
		}

		running = false;
	}

	return deltaTime;
}

unsigned int MSTimer::Read() {
	if (running) {
		unsigned int now_time = SDL_GetTicks();
		if (SDL_TICKS_PASSED(now_time, startTime)) {
			deltaTime = now_time - startTime;
		} else {
			deltaTime = 0;
		}
	}

	return deltaTime;
}
