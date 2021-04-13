#include "ResourceScript.h"

#include "Script.h"
#include "Application.h"
#include "Modules/ModuleProject.h"
#include "Utils/FileDialog.h"

void ResourceScript::Load() {
	std::string name = FileDialog::GetFileName(GetAssetFilePath().c_str());

//#ifdef _DEBUG
//	App->project->CompileProject(Configuration::DEBUG_EDITOR);
//#else
//	App->project->CompileProject(Configuration::RELEASE_EDITOR);
//#endif // _DEBUG

	script = Factory::Create(name);
}

void ResourceScript::Unload() {
	RELEASE(script);
}