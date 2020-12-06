#include "ModuleTime.h"

#include "Globals.h"
#include "Logging.h"

#include "SDL_timer.h"

#include "Leaks.h"

ModuleTime::ModuleTime()
{
	timer.Start();
}

UpdateStatus ModuleTime::PreUpdate()
{
	frame_count += 1;

	float real_time = timer.Read();
	real_time_delta_ms = real_time - real_time_last_ms;
	real_time_last_ms = real_time;

	time_delta_ms = real_time_delta_ms * time_scale;
	time_last_ms += time_delta_ms;

	log_delta_ms(real_time_delta_ms);

    return UpdateStatus::CONTINUE;
}

void ModuleTime::WaitForEndOfFrame()
{
	if (limit_framerate)
	{
		float real_time_ms = timer.Read();
		unsigned int frame_ms = real_time_ms - real_time_last_ms;
		unsigned int min_ms = 1000 / max_fps;
		if (frame_ms < min_ms)
		{
			SDL_Delay(min_ms - frame_ms);
		}
	}
}

float ModuleTime::GetDeltaTime() const
{
	return time_delta_ms / 1000.0f;
}

float ModuleTime::GetRealTimeDeltaTime() const
{
	return real_time_delta_ms / 1000.0f;
}

float ModuleTime::GetTimeSinceStartup()
{
	unsigned int real_time_ms = timer.Read();
	unsigned int current_real_time_delta_ms = real_time_ms - real_time_last_ms;
	unsigned int current_time_delta_ms = current_real_time_delta_ms * time_scale;
	return (time_last_ms + current_time_delta_ms) / 1000.0f;
}

float ModuleTime::GetRealTimeSinceStartup()
{
	return timer.Read() / 1000.0f;
}

float ModuleTime::GetTimeScale() const
{
	return time_scale;
}

void ModuleTime::SetTimeScale(float time_scale)
{
	time_scale = std::max(0.0f, time_scale);
}

unsigned int ModuleTime::GetFrameCount() const
{
	return frame_count;
}
