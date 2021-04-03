#pragma once

#include "Module.h"
#include "Utils/MSTimer.h"

class ModuleTime : public Module {
public:
	ModuleTime();
	bool Init() override;
	UpdateStatus PreUpdate() override;
	void ReceiveEvent(const Event& e) override;

	void WaitForEndOfFrame(); // Calculates the time until the next frame if there is a framerate limitation, and introduces a Delay for that remainign time.

	// ----- Getters & Setters ----- //
	bool HasGameStarted() const;
	bool IsGameRunning() const;

	float GetDeltaTime() const;
	float GetRealTimeDeltaTime() const;
	float GetTimeSinceStartup() const;
	float GetRealTimeSinceStartup() const;

	long long GetCurrentTimestamp() const;

	float GetTimeScale() const;
	void SetTimeScale(float timeScale);
	unsigned int GetFrameCount() const;

	// --- Game Time Controllers --- //
	// This functions control the flow of the time in-game by setting the Game Time Flags (see below).
	// They are also in charge to serialise and load the scene on play, pause and stops.
	void StartGame();
	void StopGame();
	void PauseGame();
	void ResumeGame();
	void StepGame();

public:
	int maxFps = 60;			// Maximum FPS when the framerate is limited.
	bool limitFramerate = true; // "Flag" to limit the framerate.
	bool vsync = true;			// "Flag" to force vertical sync.
	int stepDeltaTimeMs = 100;	// When calling StepGame(), the game will advance one frame, with a specific time increment of 'stepDeltaTimeMs'.

private:
	MSTimer timer = MSTimer(); // Real time Timer that is the base of every time calculation.

	unsigned int timeDeltaMs = 0;	  // Registers the time increment in the game at each frame.
	unsigned int realTimeDeltaMs = 0; // Registers the time increment at each frame.
	unsigned int timeLastMs = 0;	  // Registers the total time since the Game was started.
	unsigned int realTimeLastMs = 0;  // Registers the total time since the Application was started.
	float timeScale = 1.0f;			  // Multiplier of Game Time. 1=normal time, <1=slow motion, >1=accelerated
	unsigned int frameCount = 0;	  // Total number of frames elapsed since the Application start.

	// ------ Game Time Flags ------ //
	bool gameStarted = false;
	bool gameRunning = false;
	bool gameStepOnce = false;
};
