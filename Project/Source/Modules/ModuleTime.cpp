#include "ModuleTime.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "FileSystem/SceneImporter.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleEventSystem.h"
#include "SDL_timer.h"
#include "Brofiler.h"
#include <ctime>

#include "Utils/Leaks.h"

#define TEMP_SCENE_FILE_NAME "_scene_snapshot.temp"

ModuleTime::ModuleTime() {
	timer.Start();
}

bool ModuleTime::Init() {
	App->eventSystem->AddObserverToEvent(Event::EventType::PRESSED_PAUSE, this);
	App->eventSystem->AddObserverToEvent(Event::EventType::PRESSED_PLAY, this);
	App->eventSystem->AddObserverToEvent(Event::EventType::PRESSED_RESUME, this);
	App->eventSystem->AddObserverToEvent(Event::EventType::PRESSED_STEP, this);
	App->eventSystem->AddObserverToEvent(Event::EventType::PRESSED_STOP, this);
	return true;
}

UpdateStatus ModuleTime::PreUpdate() {
	BROFILER_CATEGORY("ModuleTime - PreUpdate", Profiler::Color::Black)

	frameCount += 1;

	unsigned int realTime = timer.Read();
	realTimeDeltaMs = realTime - realTimeLastMs;
	realTimeLastMs = realTime;

	if (gameRunning) {
		timeDeltaMs = lroundf(realTimeDeltaMs * timeScale);
		timeLastMs += timeDeltaMs;
	} else if (gameStepOnce) {
		timeDeltaMs = stepDeltaTimeMs;
		timeLastMs += timeDeltaMs;

		gameStepOnce = false;
	} else {
		timeDeltaMs = 0;
	}

	LogDeltaMS((float) realTimeDeltaMs);

	return UpdateStatus::CONTINUE;
}

void ModuleTime::WaitForEndOfFrame() {
	BROFILER_CATEGORY("ModuleTime - WaitForEndOfFrame", Profiler::Color::Black)
	if (limitFramerate) {
		unsigned int realTimeMs = timer.Read();
		unsigned int frameMs = realTimeMs - realTimeLastMs;
		unsigned int minMs = 1000 / maxFps;
		if (frameMs < minMs) {
			SDL_Delay(minMs - frameMs);
		}
	}
}

bool ModuleTime::HasGameStarted() const {
	return gameStarted;
}

bool ModuleTime::IsGameRunning() const {
	return gameRunning;
}

float ModuleTime::GetDeltaTime() const {
	return timeDeltaMs / 1000.0f;
}

float ModuleTime::GetRealTimeDeltaTime() const {
	return realTimeDeltaMs / 1000.0f;
}

float ModuleTime::GetTimeSinceStartup() const {
	return timeLastMs / 1000.0f;
}

float ModuleTime::GetRealTimeSinceStartup() const {
	return realTimeLastMs / 1000.0f;
}

long long ModuleTime::GetCurrentTimestamp() const {
	return std::time(0);
}

unsigned int ModuleTime::GetFrameCount() const {
	return frameCount;
}

void ModuleTime::StartGame() {
	if (gameStarted) return;

	// TODO: (Scene resource) Save temp scenes
	// SceneImporter::SaveScene(TEMP_SCENE_FILE_NAME);

	gameStarted = true;
	gameRunning = true;
}

void ModuleTime::StopGame() {
	if (!gameStarted) return;

	// TODO: (Scene resource) Load temp scenes
	// SceneImporter::LoadScene(TEMP_SCENE_FILE_NAME);
	std::string tempSceneFilePath = std::string(SCENES_PATH) + "/" + TEMP_SCENE_FILE_NAME + SCENE_EXTENSION;
	App->files->Erase(tempSceneFilePath.c_str());

	gameStarted = false;
	gameRunning = false;

	timeLastMs = 0;
}

void ModuleTime::PauseGame() {
	if (!gameStarted) return;
	if (!gameRunning) return;

	gameRunning = false;
}

void ModuleTime::ResumeGame() {
	if (!gameStarted) return;
	if (gameRunning) return;

	gameRunning = true;
}

void ModuleTime::StepGame() {
	if (!gameStarted) StartGame();
	if (gameRunning) PauseGame();

	gameStepOnce = true;
}

void ModuleTime::ReceiveEvent(const Event& e) {
	switch (e.type) {
	case Event::EventType::PRESSED_PLAY:
		StartGame();
		break;
	case Event::EventType::PRESSED_STOP:
		StopGame();
		break;
	case Event::EventType::PRESSED_RESUME:
		ResumeGame();
		break;
	case Event::EventType::PRESSED_PAUSE:
		PauseGame();
		break;
	case Event::EventType::PRESSED_STEP:
		StepGame();
		break;
	default:
		break;
	}
}