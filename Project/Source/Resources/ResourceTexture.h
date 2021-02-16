#pragma once

#include "Resource.h"

#include <string>

class ResourceTexture : public Resource {
public:
	ResourceTexture(UID id, std::string localFilePath);

	bool Import() override;
	void Delete() override;

protected:
	void Load() override;
	void Unload() override;

public:
	unsigned glTexture = 0;
};