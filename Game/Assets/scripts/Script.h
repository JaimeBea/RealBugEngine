#pragma once

#include "FileSystem/JsonValue.h"

namespace ScriptImporter {
	bool ImportScript(const char* filePath, JsonValue jMeta);
	bool ImportScript(const char* filePath);
} // namespace ScriptImporter