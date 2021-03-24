#pragma once
#include "Module.h"

#include "Components/ComponentAudioSource.h"

#include "AL/alc.h"

#define NUM_SOURCES 16

class ModuleAudio : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;
	bool CleanUp() override;

	ALuint GetAvailableSource(bool reverse = false) const;
	bool isActive(ALuint sourceId) const;
	bool isAvailable(ALuint sourceId) const;

private:
	ALCdevice* openALDevice = nullptr;
	ALCcontext* openALContext = nullptr;
	ALCboolean contextMadeCurrent = false;
	ALuint sources[NUM_SOURCES] = {0};
};