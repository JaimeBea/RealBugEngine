#pragma once

#include "FileSystem/JsonValue.h"

class ResourceShader;

namespace ShaderImporter {
	bool ImportShader(const char* filePath, JsonValue jMeta);
} // namespace ShaderImporter