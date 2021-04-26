#pragma once

#include "Module.h"

#define NUM_SOURCES 16

struct ALCdevice;
struct ALCcontext;

class ModuleAudio : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;
	bool CleanUp() override;

	unsigned GetAvailableSource(bool reverse = false) const;
	bool isActive(unsigned sourceId) const;
	bool isAvailable(unsigned sourceId) const;

private:
	ALCdevice* openALDevice = nullptr;
	ALCcontext* openALContext = nullptr;
	bool contextMadeCurrent = false;
	unsigned sources[NUM_SOURCES] = {0};
};