#include "ResourceScript.h"

#include "Script.h"
#include "Application.h"
#include "Modules/ModuleProject.h"
#include "Utils/FileDialog.h"

void ResourceScript::Load() {
	std::string name = FileDialog::GetFileName(GetAssetFilePath().c_str());
	script = Factory::Create(name);
}

void ResourceScript::Unload() {
	RELEASE(script);
}