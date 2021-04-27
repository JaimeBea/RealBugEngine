#pragma once

#include "Components/ComponentType.h"
#include "FileSystem/JsonValue.h"
#include "Utils/UID.h"

class GameObject;

#if defined(TESSERACT_ENGINE_API)
/* do nothing. */
#elif defined(_MSC_VER)
#define TESSERACT_ENGINE_API __declspec(dllexport)
#endif

class Component {
public:
	Component(ComponentType type, GameObject* owner, UID id, bool active);
	virtual ~Component();

	// ------- Core Functions ------ //
	virtual void Init();								// Performs the initialisations of the Component that are detached from the constructora. Called from owner->InitComponents().
	virtual void Update();								// Updates the Component at each frame. Called on owner->Update()
	virtual void DrawGizmos();							// Draws the visual representation of the component in the screen (if exists, I.E. Light direction or camera frustum).
	virtual void OnEditorUpdate();						// Draw the ImGui elements & info of the Component in the Inspector. Called from PanelInspector->Update()
	virtual void Save(JsonValue jComponent) const;		// Operations to serialise this Component when saving the scene. Called from owner->Save().
	virtual void Load(JsonValue jComponent);			// Operations to initialise this Component when a scene is loaded. Called from owner->Load().
	virtual void DuplicateComponent(GameObject& owner); // Used when duplicating GameObjects. It duplicates this Component into a new Component in owner.

	// ---- Visibility Setters ----- //
	TESSERACT_ENGINE_API void Enable();
	TESSERACT_ENGINE_API void Disable();

	// ---- Virtual Enable/Disable callbacks ----//
	virtual void OnEnable() {}
	virtual void OnDisable() {}

	// ---- Prefab Functions ----- //
	bool IsEdited();	// Returs the edited variable
	void SetEdited();	// Sets edited to true. Set this every time a component member is changed.
	void ResetEdited(); // Sets edited to false. Only for internal use.

	// ---------- Getters ---------- //
	ComponentType GetType() const;
	GameObject& GetOwner() const;
	UID GetID() const;
	TESSERACT_ENGINE_API bool IsActive() const;
	bool IsActiveInHierarchy() const;

protected:
	ComponentType type = ComponentType::UNKNOWN; // See ComponentType.h for a list of all available types.

private:
	UID id = 0;					 // Unique identifier for the component
	bool active = true;			 // Visibility of the Component. If active is false the GameObject behaves as if this Component doesn't exist.
	bool edited = true;			 // This boolean saves whether the component was edited in a prefab or not. It only makes sense inside prefabs.
	GameObject* owner = nullptr; // References the GameObject this Component applies its functionality to. Its 'parent'.
};
