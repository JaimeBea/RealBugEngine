#pragma once
#include "FileSystem/JsonValue.h"

namespace ModelImporter {
	bool ImportSateMachine(const char* filePath, JsonValue jMeta);
} // namespace ModelImporter
