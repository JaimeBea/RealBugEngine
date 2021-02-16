#pragma once

#include "Utils/UID.h"

#include <string>

class Resource {
public:
	Resource(UID id, const char* assetFilePath, const char* resourceFilePath);
	virtual ~Resource();

	void IncreaseReferenceCount();
	void DecreaseReferenceCount();

	UID GetId();
	const std::string& GetAssetFilePath();
	const std::string& GetResourceFilePath();

	virtual void Load();
	virtual void Unload();

private:
	UID id = 0;
	std::string assetFilePath = "";
	std::string resourceFilePath = "";
	int referenceCount = 0;
};
