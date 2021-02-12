#pragma once

class GameObject;
class Component;

/* Creating a new component type:
*    1. Add a new ComponentType for the new component
*    2. Add REGISTER_COMPONENT to the .h of the new component
*    3. Add the new component to the CreateComponentByType function in ComponentType.cpp
*/

#define REGISTER_COMPONENT(componentClass, componentType)  \
	static const ComponentType staticType = componentType; \
	componentClass(GameObject& owner, bool active) : Component(staticType, owner, active) {}

enum class ComponentType {
	// SERIALIZATION: IDs should be consistent between versions (don't change what an ID means)
	UNKNOWN = 0,
	TRANSFORM = 1,
	MESH = 2,
	MATERIAL = 3,
	BOUNDING_BOX = 4,
	CAMERA = 5,
	LIGHT = 6,
};

Component* CreateComponentByType(GameObject& owner, ComponentType type, bool active = true);