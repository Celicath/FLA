#pragma once

#include "platform.h"

class screen {
public:
	virtual void update() {}
	virtual void draw() {}
	virtual void redraw() {}

	virtual int routine() { return -1; }
};