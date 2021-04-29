#include "ResourceScript.h"

#include "Script.h"
#include "Application.h"
#include "Modules/ModuleProject.h"
#include "Utils/FileDialog.h"

void ResourceScript::Load() {
	name = FileDialog::GetFileName(GetAssetFilePath().c_str());
}

void ResourceScript::Unload() {
	name = "";
}