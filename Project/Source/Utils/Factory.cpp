#include "Factory.h"

#include "Creator.h"

void Factory::RegisterScript(const std::string& className, Creator* creator) {
	Get_table()[className] = creator;
}

Script* Factory::Create(const std::string& className) {
	auto it = Get_table().find(className);
	if (it != Get_table().end()) {
		return it->second->Create();
	} else {
		return (Script*) nullptr;
	}
}

std::map < std::string, Creator*> &Factory::Get_table() {
	static std::map<std::string, Creator*> table;
	return table;
}