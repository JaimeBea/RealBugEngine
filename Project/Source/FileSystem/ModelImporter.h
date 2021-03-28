#pragma once

#include "FileSystem/JsonValue.h"

class GameObject;

namespace ModelImporter {
	bool ImportModel(const char* filePath, JsonValue jMeta);

	bool SavePrefab(const char* filePath, JsonValue jMeta, GameObject* root, unsigned& resourceIndex);
} // namespace ModelImporter