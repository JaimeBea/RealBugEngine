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

	bool HasGameStarted() const;
	bool IsGameRunning() const;

	void StartGame();
	void StopGame();
	void PauseGame();
	void ResumeGame();
	void StepGame();

	unsigned int GetFrameCount() const;

public:
	int max_fps = 60;
	bool limit_framerate = true;
	bool vsync = true;
	int step_delta_time_ms = 100;

private:
	MSTimer timer = MSTimer();

	unsigned int frame_count = 0;

	unsigned int real_time_delta_ms = 0;
	unsigned int real_time_last_ms = 0;

	float time_scale = 1.0f;
	unsigned int time_delta_ms = 0;
	unsigned int time_last_ms = 0;

	bool game_started = false;
	bool game_running = false;
	bool game_step_once = false;
};

