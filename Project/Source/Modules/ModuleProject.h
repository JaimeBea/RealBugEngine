#pragma once

#include "Module.h"

#include "Utils/UID.h"

#include <string>

#ifndef _WINDEF_
struct HINSTANCE__; // Forward or never
typedef HINSTANCE__* HINSTANCE;
typedef HINSTANCE HMODULE;
#endif

enum class Configuration {
	RELEASE,
	RELEASE_EDITOR,
	DEBUG,
	DEBUG_EDITOR
};

class ModuleProject : public Module {
public:
	bool Init() override;
	bool CleanUp() override;

	void CreateScript(std::string& name);
	void CreateNewProject(const char* name, const char* path);
	void LoadProject(const char* path);

	void CompileProject(Configuration config);

public:
	HMODULE gameCodeDLL = nullptr;
	std::string projectName = "";
	std::string projectPath = "";

private:
	bool LoadGameCodeDLL(const char* path);
	bool UnloadGameCodeDLL();
	void CreateMSVCSolution(const char* path, const char* name, const char* UIDProject);
	void CreateMSVCProject(const char* path, const char* name, const char* UIDProject);
	void CreateBatches();

};
