#pragma once

#include "Resources/Resource.h"

class ResourceScene : public Resource {
public:
	ResourceScene(UID id, std::string localFilePath);

	bool Import() override;
	void Delete() override;

protected:
	void Load() override;
	void Unload() override;
};
