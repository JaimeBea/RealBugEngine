#pragma once

#include "Utils/UID.h"
#include "FileSystem/JsonValue.h"

#include <string>

class Resource {
public:
	Resource(UID id, std::string localFilePath);
	virtual ~Resource();

	virtual bool Import();
	virtual void Delete();

	virtual void SaveImportSettings(JsonValue jMeta);
	virtual void LoadImportSettings(JsonValue jMeta);

	void IncreaseReferenceCount();
	void DecreaseReferenceCount();

	UID GetId();
	const std::string& GetLocalFilePath();

protected:
	virtual void Load();
	virtual void Unload();

protected:
	UID id = 0;
	std::string localFilePath = "";
	int referenceCount = 0;
};
