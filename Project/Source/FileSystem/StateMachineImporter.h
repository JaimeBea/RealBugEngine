#pragma once
#include "FileSystem/JsonValue.h"

class StateMachineImporter {
	bool ImportSateMachine(const char* filePath, JsonValue jMeta);
};
