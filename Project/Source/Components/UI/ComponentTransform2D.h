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

// Transform in the 2D Space. Used for UI Components.
class ComponentTransform2D : public Component {
public:
	REGISTER_COMPONENT(ComponentTransform2D, ComponentType::TRANSFORM2D, false);
	void Update() override;							// Calculates the Global Matrix
	void OnEditorUpdate() override;					// Works as input of the different values of the component
	void Save(JsonValue jComponent) const override; // Serializes Component
	void Load(JsonValue jComponent) override;		// Deserializes Component
	void DrawGizmos() override;

	TESSERACT_ENGINE_API void SetPosition(float3 position);			// Sets this position to value
	TESSERACT_ENGINE_API void SetSize(float2 size);					// Sets this size to value
	void SetRotation(Quat rotation);								// Sets this rotation to value and calculates Euler Angles rotation
	void SetRotation(float3 rotation);								// Sets this eulerAngles to value and calculates Quat rotation
	void SetScale(float3 scale);									// Sets this scale to value
	void SetAnchorX(float2 anchorX);								// Sets this anchorX to value
	void SetAnchorY(float2 anchorY);								// Sets this anchorY to value
	void SetPivot(float2 pivot);									// Sets this pivot to value
	const float4x4 GetGlobalMatrix();								// Returns GlobalMatrix
	const float4x4 GetGlobalMatrixWithSize(bool isRunning = false); // Returns GlobalMatrix with the size of the item

	TESSERACT_ENGINE_API float3 GetPosition() const; // Returns the position
	TESSERACT_ENGINE_API float2 GetSize() const; // Returns the size
	float3 GetScale() const;					 // Returns the scale

	void InvalidateHierarchy();							 // Invalidates hierarchy
	void Invalidate();									 // Invalidates component
	void DuplicateComponent(GameObject& owner) override; // Duplicates component (THIS SHOULDN'T BE USED)

private:
	float2 pivot = float2(0.5, 0.5); // The position of the pivot
	float2 size = float2(200, 200);	 // The size of the item

	float3 position = float3::zero;			// The offset position
	Quat rotation = Quat::identity;			// The rotation of the element in Quaternion
	float3 localEulerAngles = float3::zero; // The rotation of the element in Euler
	float3 scale = float3::one;				// The scale of the element

	float2 anchorX = float2::zero; // The Anchor of X axis. AnchorX.x -> Min position, AnchorX.y -> Max position. Currently not used.
	float2 anchorY = float2::zero; // The Anchor of Y axis. AnchorY.x -> Min position, AnchorY.y -> Max position. Currently not used.

	float4x4 localMatrix = float4x4::identity;	// Local matrix
	float4x4 globalMatrix = float4x4::identity; // Global Matrix

	bool dirty = true;

	void CalculateGlobalMatrix(); // Calculates the Global Matrix
	void UpdateUIElements();	// If the transform changes, is gonna update UI Elements that need to recalculate vertices (p.e: ComponentText RecalculateVertices)
};
