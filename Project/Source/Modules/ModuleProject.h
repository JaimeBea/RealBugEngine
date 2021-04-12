#pragma once

#include "Module.h"

#include "Utils/UID.h"

#include <string>

enum class Configuration {
	RELEASE,
	RELEASE_EDITOR,
	DEBUG,
	DEBUG_EDITOR
};

class ModuleProject : public Module {
public:
	bool Init() override;

#ifndef GAME

	void CreateScript(std::string& name);
	void CreateNewProject(const char* name, const char* path);
	void LoadProject(const char* path);

	void CompileProject(Configuration config);

private:
	void CreateMSVCSolution(const char* path, const char* name, const char* UIDProject);
	void CreateMSVCProject(const char* path, const char* name, const char* UIDProject);
	void CreateBatches();

public:
	std::string projectName = "";
	std::string projectPath = "";

#endif // !GAME
};
