#include "EngineAPI.h"

#include "Logging.h"

#include <Windows.h>

namespace {
	static HMODULE gameCodeDLL = nullptr;
}

bool LoadGameCodeDLL(const char* path) {
	if (gameCodeDLL) {
		LOG("DLL already loaded.");
		return false;
	}

	gameCodeDLL = LoadLibraryA(path);

	return gameCodeDLL ? true : false;
}

bool UnloadGameCodeDLL() {
	if (!gameCodeDLL) {
		LOG("No DLL to unload.");
		return false;
	}

	FreeLibrary(gameCodeDLL);

	gameCodeDLL = nullptr;

	return true;
}