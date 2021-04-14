#include "ModuleProject.h"

#include "Application.h"
#include "ModuleFiles.h"
#include "FileSystem/JsonValue.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/UID.h"
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

static std::string GetLastErrorStdStr() {
	DWORD error = GetLastError();
	if (error) {
		LPVOID lpMsgBuf;
		DWORD bufLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		if (bufLen) {
			LPCSTR lpMsgStr = (LPCSTR) lpMsgBuf;
			std::string result(lpMsgStr, lpMsgStr + bufLen);

			LocalFree(lpMsgBuf);

			return result;
		}
	}
	return std::string();
}

bool ModuleProject::Init() {
#if GAME
	UnloadGameCodeDLL();
	if (!LoadLibrary("Penteract/Build/ReleaseEditor/Penteract.dll")) {
		LOG("%s", GetLastErrorStdStr().c_str());
	}
#else
	LoadProject("Penteract/Penteract.sln");
#endif
	return true;
}

bool ModuleProject::CleanUp() {
	UnloadGameCodeDLL();
	return true;
}

void ModuleProject::LoadProject(const char* path) {
	projectPath = FileDialog::GetFileFolder(path);
	projectName = FileDialog::GetFileNameAndExtension(path);
	App->files->AddSearchPath(projectPath.c_str());

	if (!App->files->Exists(projectName.c_str())) {
		CreateNewProject(projectPath.c_str(), "");
	}

#ifdef _DEBUG
	CompileProject(Configuration::DEBUG_EDITOR);
#else
	CompileProject(Configuration::RELEASE_EDITOR);
#endif // _DEBUG
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
	std::string result = fmt::format(project, name, UIDProject, "../../Project/Source/", "../../Project/Libs/MathGeoLib", "../../Project/Libs/SDL/include", "../../Project/Libs/rapidjson/include", enginePath);
#else
	std::string result = fmt::format(project, name, UIDProject, "../../Project/Source/", "../../Project/Libs/MathGeoLib", "../../Project/Libs/SDL/include", "../../Project/Libs/rapidjson/include", enginePath);
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
	UnloadGameCodeDLL();
	if (!LoadGameCodeDLL(buildPath.c_str())) {
		LOG("DLL NOT LOADED");
	};
}

bool ModuleProject::LoadGameCodeDLL(const char* path) {
	if (gameCodeDLL) {
		LOG("DLL already loaded.");
		return false;
	}

	gameCodeDLL = LoadLibraryA(path);

	return gameCodeDLL ? true : false;
}

bool ModuleProject::UnloadGameCodeDLL() {
	if (!gameCodeDLL) {
		LOG("No DLL to unload.");
		return false;
	}

	FreeLibrary(gameCodeDLL);

	gameCodeDLL = nullptr;

	return true;
}