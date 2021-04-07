#pragma once

/* Creating a new component type:
*    1. Add a new ComponentType for the new component
*    2. Add REGISTER_COMPONENT to the .h of the new component
*    3. Create a new VectorMap for the new component in Scene.h
*    4. Add the new component to the ByTypeAndId functions in GameObject.cpp
*    5. Implement DuplicateComponent for the new component
*    6. Add the "Add Component" functionality in PanelInspector.cpp (ln. 62)
*/

// REGISTER_COMPONENT builds the data structures common to all Components.
// This includes the Constructor.
#define REGISTER_COMPONENT(componentClass, componentType, allowMultiple) \
	static const ComponentType staticType = componentType;               \
	static const bool allowMultipleComponents = allowMultiple;           \
	componentClass(GameObject* owner, UID id, bool active) : Component(staticType, owner, id, active) {}

enum class ComponentType {
	// SERIALIZATION: IDs should be consistent between versions (don't change what an ID means)
	UNKNOWN = 0,
	TRANSFORM = 1,
	MESH_RENDERER = 2,
	BOUNDING_BOX = 3,
	CAMERA = 4,
	LIGHT = 5,
	SKYBOX = 6,
	SCRIPT = 7,
	ANIMATION = 8
};
