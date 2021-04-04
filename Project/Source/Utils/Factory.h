#pragma once

#include "Globals.h"

#include <string>
#include <map>

class Script;
class Creator;

class TESSERACT_ENGINE_API Factory {
public:
	static Script* create(const std::string& className);
	static void registerScript(const std::string& classname, Creator* creator);

private:
	static std::map<std::string, Creator*>& get_table();
};
