#include "ComponentType.h"

#include "Resources/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMesh.h"
#include "Components/ComponentMaterial.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentDirectionalLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"

#include "Utils/Leaks.h"

Component* CreateComponentByType(GameObject& game_object, ComponentType type, bool active)
{
	switch (type)
	{
	case ComponentType::TRANSFORM:
		return game_object.CreateComponent<ComponentTransform>(active);
	case ComponentType::MESH:
		return game_object.CreateComponent<ComponentMesh>(active);
	case ComponentType::MATERIAL:
		return game_object.CreateComponent<ComponentMaterial>(active);
	case ComponentType::BOUNDING_BOX:
		return game_object.CreateComponent<ComponentBoundingBox>(active);
	case ComponentType::CAMERA:
		return game_object.CreateComponent<ComponentCamera>(active);
	case ComponentType::DIRECTIONAL_LIGHT:
		return game_object.CreateComponent<ComponentDirectionalLight>(active);
	case ComponentType::POINT_LIGHT:
		return game_object.CreateComponent<ComponentPointLight>(active);
	case ComponentType::SPOT_LIGHT:
		return game_object.CreateComponent<ComponentSpotLight>(active);
	default:
		return nullptr;
	}

	return nullptr;
}
