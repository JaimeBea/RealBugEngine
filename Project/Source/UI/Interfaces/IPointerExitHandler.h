#pragma once

class IPointerExitHandler { //Interface  that must be implemented by those SelectableComponents that we want the OnPointerExit to affect

public:
	virtual ~IPointerExitHandler() {}
	virtual void OnPointerExit() = 0;
};