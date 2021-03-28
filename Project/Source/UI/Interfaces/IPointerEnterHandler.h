#pragma once
class IPointerEnterHandler {
public:
	virtual ~IPointerEnterHandler() {}
	virtual void OnPointerEnter() = 0;
};