#pragma once

#include "Resources/Resource.h"

class ResourceShader : public Resource {
public:
	ResourceShader(UID id, std::string localFilePath);

	bool Import() override;
	void Delete() override;

protected:
	void Load() override;
	void Unload() override;
};
