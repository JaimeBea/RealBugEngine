#pragma once

#include "Math/float2.h"
#include <unordered_map>

class ResourceFont;

namespace FontImporter {
	// Change to Resource
	ResourceFont* ImportFont(const std::string& path);
}; //namespace FontImporter