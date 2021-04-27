#pragma once

#include "FileSystem/JsonValue.h"

namespace PrefabImporter {
	bool ImportPrefab(const char* filePath, JsonValue jMeta);
} // namespace PrefabImporter