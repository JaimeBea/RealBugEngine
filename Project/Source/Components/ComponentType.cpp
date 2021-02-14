#include "ComponentType.h"

#include "Resources/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentMaterial.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"

#include "Utils/Leaks.h"

Component* CreateComponentByType(GameObject& owner, ComponentType type, bool active) {
	switch (type) {
	case ComponentType::TRANSFORM:
		return owner.CreateComponent<ComponentTransform>(active);
	case ComponentType::MESH:
		return owner.CreateComponent<ComponentMeshRenderer>(active);
	case ComponentType::MATERIAL:
		return owner.CreateComponent<ComponentMaterial>(active);
	case ComponentType::BOUNDING_BOX:
		return owner.CreateComponent<ComponentBoundingBox>(active);
	case ComponentType::CAMERA:
		return owner.CreateComponent<ComponentCamera>(active);
	case ComponentType::LIGHT:
		return owner.CreateComponent<ComponentLight>(active);
	default:
		return nullptr;
	}

	return nullptr;
}
