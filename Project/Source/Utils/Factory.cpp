#include "Factory.h"

#include "Creator.h"

void Factory::registerScript(const std::string& className, Creator* creator) {
	get_table()[className] = creator;
}

Script* Factory::create(const std::string& className) {
	auto it = get_table().find(className);
	if (it != get_table().end()) {
		return it->second->create();
	} else {
		return (Script*) nullptr;
	}
}

std::map < std::string, Creator*> &Factory::get_table() {
	static std::map<std::string, Creator*> table;
	return table;
}