#pragma once
class IPointerEnterHandler { //Interface  that must be implemented by those SelectableComponents that we want the OnPointerEnter to affect
public:
	virtual ~IPointerEnterHandler() {}
	virtual void OnPointerEnter() = 0;
};