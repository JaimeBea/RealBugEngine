#pragma once

#include "FileSystem/JsonValue.h"

namespace SkyboxImporter {
	bool ImportSkybox(const char* filePath, JsonValue jMeta);
};
