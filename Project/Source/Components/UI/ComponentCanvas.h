#pragma once

#include "Components/Component.h"
#include "Math/float2.h"
class ComponentCanvas : public Component {
public:
	REGISTER_COMPONENT(ComponentCanvas, ComponentType::CANVAS, false);

	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void Init() override;
	void Update() override;
	void DuplicateComponent(GameObject& owner) override;
	void OnEditorUpdate() override;
	bool CanBeRemoved() const override;

	void SetScreenReferenceSize(float2 screenReferenceSize_);
	void SetDirty(bool dirty_);	   //Dirty marks wether or not hte next time GetScreenFactor is called will be recalculated or not
	float GetScreenFactor() const; //Returns the factor by which UI elements will be scaled (both x and y so it doesn't deform elements)

private:
	void RecalculateScreenFactor();
	bool AnyChildHasCanvasRenderer(const GameObject*) const; //This method returns true if any child has a canvas renderer, it is only meant to be used to check if the canvas component can be destroyed

private:
	float2 screenReferenceSize = float2(1920, 1080);
	float screenFactor = 0.0f;
	bool dirty = true;
};
