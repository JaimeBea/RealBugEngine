#pragma once
#include "Component.h"
class AnimationController;
//class AnimationResource;

class ComponentAnimation : public Component {

public:
	void Init() override;
	void Update() override;
	void DrawGizmos() override; 
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void OnStop();
	void OnPlay();
	void OnUpdate();

	AnimationController* animationController;
	//AnimationResource* animationResource;

private:
	void UpdateAnimations(GameObject *gameObject);
};
