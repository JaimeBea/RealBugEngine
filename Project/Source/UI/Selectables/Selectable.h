#pragma once
#include <Math/float2.h>
#include <vector>
#include <Components/Component.h>
class ModuleEventSystem;
class Selectable : public Component {
public:
	enum class NavigationType {
		NONE,
		AUTOMATIC,
		MANUAL
	};

	//TO DO
	enum class TransitionType {
		COLOR_CHANGE,
		ANIMATION,
		NONE
	};

protected:
	bool interactable;
	bool highlighted;
	bool selected;
	int selectableIndex;
	NavigationType m_NavigationType;
	TransitionType m_Transition;
	void Highlight(bool b);

public:
	Selectable(GameObject& owner, bool active = true);
	~Selectable();
	bool GetInteractable();
	void SetInteractable(bool b);
	Selectable* FindSelectableOnDir(float2 dir);

	virtual void OnSelect();
	virtual void OnDeselect();
	Selectable* onAxisUp;
	Selectable* onAxisDown;
	Selectable* onAxisLeft;
	Selectable* onAxisRight;

	void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	void Enable() override;
	void Disable() override;
};
