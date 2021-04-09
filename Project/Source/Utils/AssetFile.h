#pragma once

#include "UID.h"

#include <string>
#include <vector>

struct AssetFile {
	AssetFile(const char* path);

	std::string path;
	std::vector<UID> resourceIds;
};

struct AssetFolder {
	AssetFolder(const char* path);

	std::string path;
	std::vector<AssetFolder> folders;
	std::vector<AssetFile> files;
};
