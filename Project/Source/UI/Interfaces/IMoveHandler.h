#pragma once
#include <Math/float2.h>
class IMoveHandler {
public:
	~IMoveHandler() {}
	virtual void OnMouseMoved(float2 mousePos) = 0;
};