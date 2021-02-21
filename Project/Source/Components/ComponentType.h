#pragma once

class GameObject;
class Component;

/* Creating a new component type:
*    1. Add a new ComponentType for the new component
*    2. Add REGISTER_COMPONENT to the .h of the new component
*    3. Add the new component to the CreateComponentByType function in ComponentType.cpp
*    4. Add the "Add Component" functionality in PanelInspector.cpp (ln. 62)
*/

#define REGISTER_COMPONENT(componentClass, componentType)  \
	static const ComponentType staticType = componentType; \
	componentClass(GameObject& owner, bool active) : Component(staticType, owner, active) {}

enum class ComponentType {
	// SERIALIZATION: IDs should be consistent between versions (don't change what an ID means)
	UNKNOWN = 0,
	TRANSFORM = 1,
	MESH = 2,
	BOUNDING_BOX = 3,
	CAMERA = 4,
	LIGHT = 5,
};

Component* CreateComponentByType(GameObject& owner, ComponentType type, bool active = true);