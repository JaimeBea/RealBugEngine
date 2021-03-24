#pragma once

#include "FileSystem/JsonValue.h"

namespace SceneImporter {
	bool ImportScene(const char* filePath, JsonValue jMeta);

	bool SaveScene(const char* filePath);
} // namespace SceneImporter