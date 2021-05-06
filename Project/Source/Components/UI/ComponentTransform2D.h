#pragma once

#include "Components/Component.h"

#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

#if defined(TESSERACT_ENGINE_API)
/* do nothing. */
#elif defined(_MSC_VER)
#define TESSERACT_ENGINE_API __declspec(dllexport)
#endif

//TODO THIS CLASS CURRENTLY ONLY WORKS FOR OBJECTS THAT USE THE CENTER OF THE SCREEN AS THE CENTER OF COORDINATES,
//NEITHER PIVOT NOR ANCHOR ARE TAKEN INTO ACCOUNT IN ANY WAY, NOR ARE THEY MODIFIABLE LIKE THEY WOULD IN UNITY

struct AnchorPreset {
	enum class AnchorPresetType {

		// Presets
		TOP_VERTICAL_LEFT_HORIZONTAL,
		TOP_VERTICAL_CENTER_HORIZONTAL,
		TOP_VERTICAL_RIGHT_HORIZONTAL,
		TOP_VERTICAL_STRETCH_HORIZONTAL,

		MIDDLE_VERTICAL_LEFT_HORIZONTAL,
		MIDDLE_VERTICAL_CENTER_HORIZONTAL,
		MIDDLE_VERTICAL_RIGHT_HORIZONTAL,
		MIDDLE_VERTICAL_STRETCH_HORIZONTAL,

		BOTTOM_VERTICAL_LEFT_HORIZONTAL,
		BOTTOM_VERTICAL_CENTER_HORIZONTAL,
		BOTTOM_VERTICAL_RIGHT_HORIZONTAL,
		BOTTOM_VERTICAL_STRETCH_HORIZONTAL,

		STRETCH_VERTICAL_LEFT_HORIZONTAL,
		STRETCH_VERTICAL_CENTER_HORIZONTAL,
		STRETCH_VERTICAL_RIGHT_HORIZONTAL,
		STRETCH_VERTICAL_STRETCH_HORIZONTAL,

		// Custom
		CUSTOM
	};

	AnchorPreset(AnchorPresetType type_, float2 anchorMin_, float2 anchorMax_)
		: type(type_), anchorMin(anchorMin_), anchorMax(anchorMax_) {
	}

	AnchorPresetType type;
	float2 anchorMin;
	float2 anchorMax;
};

struct Rect {
	Rect(float top_, float bottom_, float left_, float right_)
		: top(top_), bottom(bottom_), left(left_), right(right_) {}

	float top, bottom, left, right;
};


// Transform in the 2D Space. Used for UI Components.
class ComponentTransform2D : public Component {
public:
	REGISTER_COMPONENT(ComponentTransform2D, ComponentType::TRANSFORM2D, false);
	void Update() override;							// Calculates the Global Matrix
	void OnEditorUpdate() override;					// Works as input of the different values of the component
	void Save(JsonValue jComponent) const override; // Serializes Component
	void Load(JsonValue jComponent) override;		// Deserializes Component
	void DrawGizmos() override;
	bool CanBeRemoved() const override; //Returns false if any UI Elements are found in this GameObject or its children

	TESSERACT_ENGINE_API void SetPosition(float3 position);			// Sets this position to value
	TESSERACT_ENGINE_API void SetSize(float2 size);					// Sets this size to value
	void SetRotation(Quat rotation);								// Sets this rotation to value and calculates Euler Angles rotation
	void SetRotation(float3 rotation);								// Sets this eulerAngles to value and calculates Quat rotation
	void SetScale(float3 scale);									// Sets this scale to value
	void SetAnchorMin(float2 anchorMin);								// Sets this anchorMin to value
	void SetAnchorMax(float2 anchorMax);								// Sets this anchorMax to value
	void SetPivot(float2 pivotPosition);							// Sets this pivot to value
	void UpdatePivotPosition();										// Update this pivot position to value
	const float4x4 GetGlobalMatrix() const;										// Returns GlobalMatrix
	const float4x4 GetGlobalMatrixWithSize(bool view3DActive = false) const;	// Returns GlobalMatrix with the size of the item. view3DActive is true when the Editor is on 3D Mode and will return the global downscaled to have a proper 3D View.

	TESSERACT_ENGINE_API float3 GetPosition() const; // Returns the position
	TESSERACT_ENGINE_API float2 GetSize() const;	 // Returns the size
	float3 GetScale() const;						 // Returns the scale
	float3 GetPivotPosition() const;			 // Returns the pivot position

	void InvalidateHierarchy();							 // Invalidates hierarchy
	void Invalidate();									 // Invalidates component
	void DuplicateComponent(GameObject& owner) override; // Duplicates component (THIS SHOULDN'T BE USED)

	void SetTop(float top);		   // Sets the right anchor
	void SetBottom(float bottom);   // Sets the right anchor
	void SetLeft(float left);	   // Sets the right anchor
	void SetRight(float right);	   // Sets the right anchor

private:
	void CalculateGlobalMatrix(); // Calculates the Global Matrix
	void UpdateUIElements();	  // If the transform changes, is gonna update UI Elements that need to recalculate vertices (p.e: ComponentText RecalculateVertices)
	bool HasAnyUIElementsInChildren(const GameObject* obj) const; //Returns true if any UI Elements are found in this GameObject or its children

private:
	float2 pivot = float2(0.5, 0.5);	 // The position of the pivot in 2D
	float3 pivotPosition = float3::zero; // The position of the pivot in the world
	float2 size = float2(200, 200);	 // The size of the item

	float3 position = float3::zero;			// The offset position
	Quat rotation = Quat::identity;			// The rotation of the element in Quaternion
	float3 localEulerAngles = float3::zero; // The rotation of the element in Euler
	float3 scale = float3::one;				// The scale of the element

	Rect anchorsRect = Rect(0, 0, 0, 0);
	static std::array<AnchorPreset, 16> anchorPresets;																	// Listwith all the possible anchors presets 
	AnchorPreset::AnchorPresetType anchorSelected = AnchorPreset::AnchorPresetType::MIDDLE_VERTICAL_CENTER_HORIZONTAL;  // The anchor selected
	bool isCustomAnchor = false;																						// Variable that checks if is selected Custom or Presets in the inspector
	float2 anchorMin = float2(0.5, 0.5);																				// The Anchor Min. Represents the lower left handle.
	float2 anchorMax = float2(0.5, 0.5);																				// The Anchor Max. Represents the upper right handle.

	float4x4 localMatrix = float4x4::identity;	// Local matrix
	float4x4 globalMatrix = float4x4::identity; // Global Matrix

	bool dirty = true;
};