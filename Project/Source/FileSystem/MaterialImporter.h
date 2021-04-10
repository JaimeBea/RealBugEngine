#pragma once

#include "FileSystem/JsonValue.h"

namespace MaterialImporter {
	bool ImportMaterial(const char* filePath, JsonValue jMeta);
	bool CreateAndSaveMaterial(const char* filePath);
} // namespace MaterialImporter