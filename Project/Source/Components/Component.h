#pragma once

#include "ComponentType.h"

class JsonValue;
class GameObject;

class Component {
public:
	Component(ComponentType type, GameObject& owner, bool active);
	virtual ~Component();

	// ------- Core Functions ------ //
	virtual void Init();						   // Performs the initialisations of the Component that are detached from the constructora. Called from owner->InitComponents().
	virtual void Update();						   // Updates the Component at each frame. Called on owner->Update()
	virtual void DrawGizmos();					   // Draws the visual representation of the component in the screen (if exists, I.E. Light direction or camera frustum).
	virtual void OnTransformUpdate();			   // Actions to be performed when the Transform of the GameObject 'owner' has changed. It is called from ComponentTransform on a change in Position, Rotation or Scale, and it is used to forward the new position, rotation and scale to the other Components of the same owner GameObject.
	virtual void OnEditorUpdate();				   // Draw the ImGui elements & info of the Component in the Inspector. Called from PanelInspector->Update()
	virtual void Save(JsonValue jComponent) const; // Operations to serialise this Component when saving the scene. Called from owner->Save().
	virtual void Load(JsonValue jComponent);	   // Operations to initialise this Component when a scene is loaded. Called from owner->Load().

	// ---- Visibility Setters ----- //
	void Enable();
	void Disable();

	// ---------- Getters ---------- //
	ComponentType GetType() const;
	GameObject& GetOwner() const;
	bool IsEnabled() const;
	bool IsActiveAndEnabled() const;

private:
	ComponentType type = ComponentType::UNKNOWN; // See ComponentType.h for a list of all available types.
	GameObject& owner;							 // References the GameObject this Component applies its functionality to. Its 'parent'.

	bool enabled = true; // Visibility of the Component. If enabled is false the GameObject behaves as if this Component doesn't exist.
};
