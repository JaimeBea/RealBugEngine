#include "ModuleProject.h"

#include "Application.h"
#include "ModuleFiles.h"
#include "FileSystem/JsonValue.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/UID.h"
#include "Utils/EngineAPI.h"
#include "Utils/FileDialog.h"

#include "Script.h"

#include "fmt/core.h"
#include "fmt/format.h"

#include <Windows.h>
#include <shellapi.h>
#include <ObjIdl.h>

#include "Utils/Leaks.h"

#define JSON_TAG_PROJECT_NAME "ProjectName"
#define JSON_TAG_SCENES "Scenes"

bool ModuleProject::Init() {
	LoadProject("Penteract/Penteract.sln");

	return true;
}

void ModuleProject::LoadProject(const char* path) {
	projectPath = FileDialog::GetFileFolder(path);
	projectName = FileDialog::GetFileNameAndExtension(path);

	if (!App->files->Exists(projectName.c_str())) {
		CreateNewProject(projectPath.c_str(), "");
	} else {
#ifdef _DEBUG
		CompileProject(Configuration::DEBUG_EDITOR);
#else
		CompileProject(Configuration::RELEASE_EDITOR);
#endif // _DEBUG
	}
}

void ModuleProject::CreateScript(std::string& name) {
	Buffer<char> bufferHeader = App->files->Load("Templates/Header");
	Buffer<char> bufferSource = App->files->Load("Templates/Source");

	std::string header = bufferHeader.Data();
	std::string source = bufferSource.Data();

	std::string assetsPath = "Assets/Scripts/";
	std::string realPath = projectPath + "/Source/";

	std::string result;

	result = fmt::format(header, name, "{", "}");
	App->files->Save((assetsPath + name + ".h").c_str(), result.data(), result.size());

	result = fmt::format(source, name, "{", "}");
	App->files->Save((assetsPath + name + ".cpp").c_str(), result.data(), result.size());
}

void ModuleProject::CreateNewProject(const char* name, const char* path) {
	std::string fullPath = std::string(path) + name;
	std::string batchPath = fullPath + "/Batches";

	App->files->CreateFolder(fullPath.c_str());
	App->files->AddSearchPath(fullPath.c_str());

	App->files->CreateFolder(batchPath.c_str());

	std::string UIDProject("{");
	UIDProject += GenerateUID128();
	UIDProject += "}";

	CreateMSVCSolution((fullPath + "/" + name + ".sln").c_str(), name, UIDProject.c_str());
	CreateMSVCProject((fullPath + "/" + name + ".vcxproj").c_str(), name, UIDProject.c_str());

	CreateBatches();
}

void ModuleProject::CreateMSVCSolution(const char* path, const char* name, const char* UIDProject) {
	Buffer<char> buffer = App->files->Load("Templates/MSVCSolution");

	std::string solution = buffer.Data();

	std::string UIDSolution("{");
	UIDSolution += GenerateUID128();
	UIDSolution += "}";

	std::string result = fmt::format(solution, name, UIDProject, UIDSolution);

	App->files->Save(path, result.data(), result.size());
}

void ModuleProject::CreateMSVCProject(const char* path, const char* name, const char* UIDProject) {
	Buffer<char> buffer = App->files->Load("Templates/MSVCProject");

	std::string project = buffer.Data();
	std::string enginePath = FileDialog::GetFileFolder(FileDialog::GetAbsolutePath("").c_str());

#ifdef _DEBUG
	std::string result = fmt::format(project, name, UIDProject, "../../Project/Source/", "../../Project/Libs/MathGeoLib", "../../Project/Libs/SDL/include", enginePath);
#else
	std::string result = fmt::format(project, name, UIDProject, enginePath + "Engine/Source", enginePath + "Engin/Libs/MatheGeoLib", enginePath + "Libs/SDL/include", enginePath + "Engine/Lib");
#endif

	App->files->Save(path, result.data(), result.size());
}

void ModuleProject::CreateBatches() {
	Buffer<char> buffer;
	std::string file;
	std::string result;

	// Debug (.exe)
	buffer = App->files->Load("Templates/BuildDebug");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildDebug.bat").c_str(), result.data(), result.size());

	// Debug Editor (.dll)
	buffer = App->files->Load("Templates/BuildDebugEditor");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildDebugEditor.bat").c_str(), result.data(), result.size());

	// Release (.exe)
	buffer = App->files->Load("Templates/BuildRelease");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildRelease.bat").c_str(), result.data(), result.size());

	// Release Editor (.dll)
	buffer = App->files->Load("Templates/BuildReleaseEditor");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildReleaseEditor.bat").c_str(), result.data(), result.size());
}

void ModuleProject::CompileProject(Configuration config) {
	UnloadGameCodeDLL();

	std::string batchDir = projectPath + "/Batches";

	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = "open";
	sei.lpParameters = NULL;
	sei.lpDirectory = batchDir.c_str();
	sei.nShow = NULL;
	sei.hInstApp = NULL;

	switch (config) {
	case Configuration::RELEASE:
		sei.lpFile = "BuildRelease.bat";
		break;
	case Configuration::RELEASE_EDITOR:
		sei.lpFile = "BuildReleaseEditor.bat";
		break;
	case Configuration::DEBUG:
		sei.lpFile = "BuildDebug.bat";
		break;
	case Configuration::DEBUG_EDITOR:
		sei.lpFile = "BuildDebugEditor.bat";
		break;
	default:
		sei.lpFile = "";
		break;
	}

	ShellExecuteEx(&sei);
	WaitForSingleObject(sei.hProcess, INFINITE);
	CloseHandle(sei.hProcess);

	std::string buildPath = projectPath;
	std::string name = FileDialog::GetFileName(projectName.c_str());

	switch (config) {
	case Configuration::RELEASE:
		buildPath += "/Build/Release/";
		break;
	case Configuration::RELEASE_EDITOR:
		buildPath += "/Build/ReleaseEditor/";
		break;
	case Configuration::DEBUG:
		buildPath += "/Build/Debug/";
		break;
	case Configuration::DEBUG_EDITOR:
		buildPath += "/Build/DebugEditor/";
		break;
	}

	buildPath += name + ".dll";
	LoadGameCodeDLL(buildPath.c_str());
}