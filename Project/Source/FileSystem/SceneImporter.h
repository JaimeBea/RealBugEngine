#pragma once

#include "FileSystem/JsonValue.h"

namespace SceneImporter {
	bool ImportScene(const char* filePath, JsonValue jMeta);
} // namespace SceneImporter