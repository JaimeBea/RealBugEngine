#pragma once

#include <string>
#include <map>

#if defined(TESSERACT_ENGINE_API)
/* do nothing. */
#elif defined(_MSC_VER)
#define TESSERACT_ENGINE_API __declspec(dllexport)
#endif

class Script;
class Creator;

class TESSERACT_ENGINE_API Factory {
public:
	static Script* create(const std::string& className);
	static void registerScript(const std::string& classname, Creator* creator);

private:
	static std::map<std::string, Creator*>& get_table();
};
