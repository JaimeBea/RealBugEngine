#pragma once

#include <string>
#include <vector>

#include "Modules/ModuleFiles.h"

namespace FileDialog {
	void Init(const std::string& title,bool editMode = false, AllowedExtensionsFlag ext = AllowedExtensionsFlag::ALL, const std::string& defaultPath = "");
	bool OpenDialog(std::string& selectedPath);
};
