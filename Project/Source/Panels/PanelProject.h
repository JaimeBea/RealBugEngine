#pragma once

#include "Panel.h"
#include "Resources/Resource.h"
#include "Modules/ModuleResources.h"

#include <string>

class PanelProject : public Panel {
public:
	PanelProject();

	void Update() override;

private:
	void UpdateFoldersRecursive(const AssetFolder& folder);
	void UpdateAssetsRecursive(const AssetFolder& folder);
	void UpdateResourcesRecursive(const AssetFolder& folder);

private:
	std::string selectedFolder = "";
	std::string selectedAsset = "";
};
