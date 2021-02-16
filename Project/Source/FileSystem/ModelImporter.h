#pragma once

#include "FileSystem/JsonValue.h"

namespace ModelImporter {
	bool ImportModel(const char* filePath, JsonValue jMeta);
} // namespace ModelImporter