#pragma once

#include "Components/ComponentType.h"
#include "Utils/UID.h"

class JsonValue;
class GameObject;

class Component {
public:
	Component(ComponentType type, GameObject* owner, UID id, bool active);
	virtual ~Component();

	// ------- Core Functions ------ //
	virtual void Init();								// Performs the initialisations of the Component that are detached from the constructora. Called from owner->InitComponents().
	virtual void Update();								// Updates the Component at each frame. Called on owner->Update()
	virtual void DrawGizmos();							// Draws the visual representation of the component in the screen (if exists, I.E. Light direction or camera frustum).
	virtual void OnTransformUpdate();					// Actions to be performed when the Transform of the GameObject 'owner' has changed. It is called from ComponentTransform->aaa() to all other Components when a change is detected.
	virtual void OnEditorUpdate();						// Draw the ImGui elements & info of the Component in the Inspector. Called from PanelInspector->Update()
	virtual void Save(JsonValue jComponent) const;		// Operations to serialise this Component when saving the scene. Called from owner->Save().
	virtual void Load(JsonValue jComponent);			// Operations to initialise this Component when a scene is loaded. Called from owner->Load().
	virtual void DuplicateComponent(GameObject& owner); // Used when duplicating GameObjects. It duplicates this Component into a new Component in owner.

	// ---- Visibility Setters ----- //
	virtual void Enable();
	virtual void Disable();

	// ---------- Getters ---------- //
	ComponentType GetType() const;
	GameObject& GetOwner() const;
	UID GetID() const;
	bool IsActive() const;
	bool IsActiveInHierarchy() const;

private:
	UID id = 0;			// Unique identifier for the component
	bool active = true; // Visibility of the Component. If active is false the GameObject behaves as if this Component doesn't exist.
	GameObject* owner = nullptr;				 // References the GameObject this Component applies its functionality to. Its 'parent'.

protected:
	ComponentType type = ComponentType::UNKNOWN; // See ComponentType.h for a list of all available types.
};
