#include "MSTimer.h"

#include "Globals.h"

#include "SDL_timer.h"

#include "Leaks.h"

void MSTimer::Start()
{
	start_time = SDL_GetTicks();
	delta_time = 0;
	running = true;
}

unsigned int MSTimer::Stop()
{
	if (running)
	{
		unsigned int now_time = SDL_GetTicks();
		if (SDL_TICKS_PASSED(now_time, start_time))
		{
			delta_time = now_time - start_time;
		}
		else
		{
			delta_time = 0;
		}

		running = false;
	}

	return delta_time;
}

unsigned int MSTimer::Read()
{
	if (running)
	{
		unsigned int now_time = SDL_GetTicks();
		if (SDL_TICKS_PASSED(now_time, start_time))
		{
			delta_time = now_time - start_time;
		}
		else
		{
			delta_time = 0;
		}
	}

	return delta_time;
}
