#include "ModuleProject.h"

#include "Application.h"
#include "ModuleFiles.h"
#include "FileSystem/JsonValue.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/UID.h"
#include "Utils/EngineAPI.h"

#include "Script.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"
#include "fmt/core.h"
#include "fmt/format.h"

#include <Windows.h>
#include <shellapi.h>
#include <ObjIdl.h>

#include "Utils/Leaks.h"

#define JSON_TAG_PROJECT_NAME "ProjectName"
#define JSON_TAG_SCENES "Scenes"


#define EXAMPLE(classname, pointer, variable, value) \
	static_cast<classname*>(pointer)->variable = value

bool ModuleProject::Init() {


	//HGLOBAL hResLoad;  // handle to loaded resource
	//HMODULE hExe;	   // handle to existing .EXE file
	//HRSRC hRes;		   // handle/ptr. to res. info. in hExe
	//HANDLE hUpdateRes; // update resource handle
	//LPVOID lpResLock;  // pointer to resource data
	//BOOL result;
	//
	////assert(LoadGameCodeDLL("TemplateProject.dll"));
	//hExe = LoadLibraryA("TemplateProject.dll");
	//
	//Script* p = Factory::create("CameraScr");
	//
	//assert(p != nullptr);
	//
	//p->Start();
	//p->speed = 10;
	//p->speed = 5;
	
	CreateNewProject("TemplateProject", "");
	std::string workingDir = App->files->GetWorkingDirectory();
	std::string sourceDir = App->files->GetWorkingDirectory();
	workingDir += "TemplateProject/TemplateProject.sln";
	sourceDir += "TemplateProject/Source";
	#ifdef _DEBUG
	workingDir = "TemplateProject/TemplateProject.sln";
	sourceDir = "TemplateProject/Source";
	#endif // 
	
	App->files->CreateFolder(sourceDir.c_str());
	ShellExecute(NULL, "open", workingDir.c_str(), NULL, NULL, SW_MAXIMIZE);

	return true;
}

void ModuleProject::LoadProject(const char* path) {
	// TODO: Load project
}

void ModuleProject::CreateNewProject(const char* name, const char* path) {
	projectName = name;

	std::string fullPath = std::string(path) + name;
	App->files->CreateFolder(fullPath.c_str());

	std::string UIDProject("{");
	UIDProject += GenerateUID128();
	UIDProject += "}";

	CreateMSVCSolution((fullPath + "/" + name + ".sln").c_str(), name, UIDProject.c_str());
	CreateMSVCProject((fullPath + "/" + name + ".vcxproj").c_str(), name, UIDProject.c_str());
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
	std::string enginePath = App->files->GetWorkingDirectory();
	#ifdef  _DEBUG
	std::string result = fmt::format(project, name, UIDProject, "../Project/Source/", enginePath);
	#else
	std::string result = fmt::format(project, name, UIDProject, enginePath + "Source", enginePath + "Lib\\");
	#endif

	App->files->Save(path, result.data(), result.size());
}

void ModuleProject::CompileProject(Configuration config) {
	/*
	std::string workingDir = App->files->GetWorkingDirectory();
	workingDir += "TemplateProject\\TemplateProject.sln";

	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = "open";
	sei.lpParameters = NULL;
	sei.lpDirectory = workingDir.c_str();
	sei.nShow = NULL;
	sei.hInstApp = NULL;

	switch (config) {
	case Configuration::RELEASE:
		sei.lpFile = "";
		break;
	case Configuration::RELEASE_EDITOR:
		sei.lpFile = "";
		break;
	case Configuration::DEBUG:
		sei.lpFile = "";
		break;
	case Configuration::DEBUG_EDITOR:
		sei.lpFile = "";
		break;
	default:
		sei.lpFile = "";
		break;
	}

	ShellExecuteEx(&sei);
	WaitForSingleObject(sei.hProcess, INFINITE);
	CloseHandle(sei.hProcess);

	switch (config) {
	case Configuration::RELEASE:
		break;
	case Configuration::RELEASE_EDITOR:
		break;
	case Configuration::DEBUG:
		break;
	case Configuration::DEBUG_EDITOR:
		break;
	default:
		break;
	}
	*/

}