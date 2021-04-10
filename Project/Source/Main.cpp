#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"

#include "SDL.h"
#include <stdlib.h>
#include "Brofiler.h"

#include "Utils/Leaks.h"

enum class MainState {
	CREATION,
	INIT,
	START,
	UPDATE,
	FINISH,
	EXIT
};

Application* App = nullptr;

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtMemState memState;
	_CrtMemCheckpoint(&memState);
#endif

	// Initialize logging
	logString = new std::string();
	
	// Game loop
	int mainReturn = EXIT_FAILURE;
	MainState state = MainState::CREATION;
	while (state != MainState::EXIT) {
		BROFILER_FRAME("Main");
		switch (state) {
		case MainState::CREATION:
			LOG("Application Creation --------------");
			App = new Application();
			state = MainState::INIT;
			break;

		case MainState::INIT:
			LOG("Application Init --------------");
			if (App->Init() == false) {
				LOG("Application Init exits with error -----");
				state = MainState::EXIT;
			} else {
				state = MainState::START;
			}
			break;

		case MainState::START:
			LOG("Application Start --------------");
			if (App->Start() == false) {
				LOG("Application Start exits with error -----");
				state = MainState::EXIT;
			} else {
				state = MainState::UPDATE;
				LOG("Application Update --------------");
			}
			break;

		case MainState::UPDATE: {
			UpdateStatus updateReturn = App->Update();

			if (updateReturn == UpdateStatus::FAIL) {
				LOG("Application Update exits with error -----");
				state = MainState::EXIT;
			}

			if (updateReturn == UpdateStatus::STOP) {
				state = MainState::FINISH;
			}
			break;
		}

		case MainState::FINISH:
			LOG("Application CleanUp --------------");
			if (App->CleanUp() == false) {
				LOG("Application CleanUp exits with error -----");
			} else {
				LOG("Application CleanUp completed successfuly -----");
				mainReturn = EXIT_SUCCESS;
			}
			state = MainState::EXIT;
			break;
		}
	}

	LOG("Bye :)\n");

	RELEASE(App);
	RELEASE(logString);

#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&memState);
#endif

	return mainReturn;
}
