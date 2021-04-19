#pragma once

#include "FileSystem/JsonValue.h"
#include "AL/al.h"
namespace AudioImporter {
	bool ImportAudio(const char* filePath, JsonValue jMeta);
};
