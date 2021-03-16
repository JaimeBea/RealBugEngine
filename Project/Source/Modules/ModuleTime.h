#pragma once

#include "Module.h"
#include "Utils/MSTimer.h"

class ModuleTime : public Module {
public:
	ModuleTime();

	UpdateStatus PreUpdate() override;

	void WaitForEndOfFrame();

	float GetDeltaTime() const;
	float GetRealTimeDeltaTime() const;

	float GetTimeSinceStartup() const;
	float GetRealTimeSinceStartup() const;

	float GetTimeScale() const;
	void SetTimeScale(float timeScale);

	bool HasGameStarted() const;
	bool IsGameRunning() const;

	void StartGame();
	void StopGame();
	void PauseGame();
	void ResumeGame();
	void StepGame();

	unsigned int GetFrameCount() const;
	void RecieveEvent(const Event& e) override;

public:
	int maxFps = 60;
	bool limitFramerate = true;
	bool vsync = true;
	int stepDeltaTimeMs = 100;

private:
	MSTimer timer = MSTimer();

	unsigned int frameCount = 0;

	unsigned int realTimeDeltaMs = 0;
	unsigned int realTimeLastMs = 0;

	float timeScale = 1.0f;
	unsigned int timeDeltaMs = 0;
	unsigned int timeLastMs = 0;

	bool gameStarted = false;
	bool gameRunning = false;
	bool gameStepOnce = false;
};
