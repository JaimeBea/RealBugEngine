#pragma once

#include <vector>

class GameObject;
class Component;

/* Creating a new component type:
*    1. Add a new ComponentType for the new component
*    2. Add REGISTER_COMPONENT to the .h of the new component
*    3. Add the new component to the includes above
*    4. Add the new component to the CreateComponentByType function in ComponentType.cpp
*/

// REGISTER_COMPONENT builds the data structures common to all Components.
// This includes the Constructor.
#define REGISTER_COMPONENT(componentClass, componentType)                                   \
	static const ComponentType staticType = componentType;                                  \
                                                                                            \
	static std::vector<componentClass*>& GetComponents() {                                  \
		static std::vector<componentClass*> components;                                     \
		return components;                                                                  \
	}                                                                                       \
                                                                                            \
	componentClass(GameObject& owner, bool active) : Component(staticType, owner, active) { \
		GetComponents().push_back(this);                                                    \
	}                                                                                       \
                                                                                            \
	~componentClass() override {                                                            \
		std::vector<componentClass*>& components = GetComponents();                         \
		for (auto it = components.begin(); it != components.end(); ++it) {                  \
			if (*it == this) {                                                              \
				components.erase(it);                                                       \
				break;                                                                      \
			}                                                                               \
		}                                                                                   \
	}

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