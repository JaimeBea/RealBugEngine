#include "ResourceSkybox.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"

#include "Utils/Logging.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

ResourceSkybox::ResourceSkybox(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceSkybox::Load() {
}

void ResourceSkybox::Unload() {
}