#pragma once

#include "FileSystem/JsonValue.h"

namespace MaterialImporter {
	bool ImportMaterial(const char* filePath, JsonValue jMeta);
} // namespace MaterialImporter