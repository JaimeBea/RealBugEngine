#include "ComponentType.h"

#include "Resources/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"

#include "Components/ComponentCanvas.h"
#include "Components/ComponentCanvasRenderer.h"
#include "Components/ComponentImage.h";
#include "Components/ComponentTransform2D.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Components/ComponentEventSystem.h"
#include "UI/Selectables/Button.h"

#include "Utils/Leaks.h"

Component* CreateComponentByType(GameObject& owner, ComponentType type, bool active) {
	switch (type) {
	case ComponentType::TRANSFORM:
		return owner.CreateComponent<ComponentTransform>(active);
	case ComponentType::MESH:
		return owner.CreateComponent<ComponentMeshRenderer>(active);
	case ComponentType::BOUNDING_BOX:
		return owner.CreateComponent<ComponentBoundingBox>(active);
	case ComponentType::CAMERA:
		return owner.CreateComponent<ComponentCamera>(active);
	case ComponentType::LIGHT:
		return owner.CreateComponent<ComponentLight>(active);
	case ComponentType::BUTTON:
		return owner.CreateComponent<Selectable>(active);
	case ComponentType::EVENT_SYSTEM:
		return owner.CreateComponent<ComponentEventSystem>(active);

	default:
		return nullptr;
	}

	return nullptr;
}
