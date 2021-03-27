#pragma once
class IPointerExitHandler {
public:
	virtual ~IPointerExitHandler() {}
	virtual void OnPointerExit() = 0;
};