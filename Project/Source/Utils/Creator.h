#pragma once

#include "Factory.h"

#include <string>

class Script;

class TESSERACT_ENGINE_API Creator {
public:
	Creator(const std::string& className) {
		Factory::registerScript(className, this);
	}
	virtual ~Creator() {};

	virtual Script* create() = 0;
};

template<class T>
class TESSERACT_ENGINE_API CreatorImplementation : public Creator {
public:
	CreatorImplementation<T>(const std::string& className) : Creator(className) {}

	virtual ~CreatorImplementation<T>() {}

	virtual Script* create() {
		return new T;
	}
};