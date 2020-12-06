#pragma once

#include "Module.h"
#include "MSTimer.h"

class ModuleTime : public Module
{
public:
	ModuleTime();

	UpdateStatus PreUpdate() override;

	void WaitForEndOfFrame();

	float GetDeltaTime() const;
	float GetRealTimeDeltaTime() const;

	float GetTimeSinceStartup();
	float GetRealTimeSinceStartup();

	float GetTimeScale() const;
	void SetTimeScale(float time_scale);

	unsigned int GetFrameCount() const;

public:
	int max_fps = 60;
	bool limit_framerate = true;
	bool vsync = true;

private:
	MSTimer timer = MSTimer();

	unsigned int frame_count = 0;

	unsigned int real_time_delta_ms = 0;
	unsigned int real_time_last_ms = 0;

	float time_scale = 1.0f;
	unsigned int time_delta_ms = 0;
	unsigned int time_last_ms = 0;
};

