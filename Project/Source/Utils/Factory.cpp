#include "Factory.h"

#include "Creator.h"

void Factory::RegisterScript(const std::string& className, Creator* creator) {
	GetTable()[className] = creator;
}

Script* Factory::Create(const std::string& className) {
	auto it = GetTable().find(className);
	if (it != GetTable().end()) {
		return it->second->Create();
	} else {
		return (Script*) nullptr;
	}
}

std::map < std::string, Creator*> &Factory::GetTable() {
	static std::map<std::string, Creator*> table;
	return table;
}