#include "ResourceType.h"

#include "Utils/Logging.h"

#include "Utils/Leaks.h"

const char* GetResourceTypeName(ResourceType type) {
	switch (type) {
	case ResourceType::MATERIAL:
		return "Material";
	case ResourceType::MESH:
		return "Mesh";
	case ResourceType::PREFAB:
		return "Prefab";
	case ResourceType::SCENE:
		return "Scene";
	case ResourceType::SHADER:
		return "Shader";
	case ResourceType::TEXTURE:
		return "Texture";
	case ResourceType::FONT:
		return "Font";
	default:
		LOG("Resource of type %i hasn't been registered in GetResourceTypeName.", (unsigned) type);
		assert(false); // ERROR: Resource type not registered
		return nullptr;
	}
}

ResourceType GetResourceTypeFromName(const char* name) {
	if (strcmp(name, "Material") == 0) {
		return ResourceType::MATERIAL;
	} else if (strcmp(name, "Mesh") == 0) {
		return ResourceType::MESH;
	} else if (strcmp(name, "Prefab") == 0) {
		return ResourceType::PREFAB;
	} else if (strcmp(name, "Scene") == 0) {
		return ResourceType::SCENE;
	} else if (strcmp(name, "Shader") == 0) {
		return ResourceType::SHADER;
	} else if (strcmp(name, "Texture") == 0) {
		return ResourceType::TEXTURE;
	} else if (strcmp(name, "Font") == 0) {
		return ResourceType::FONT;
	} else {
		LOG("No resource of name %s exists.", (unsigned) name);
		assert(false); // ERROR: Invalid name
		return ResourceType::UNKNOWN;
	}
}
