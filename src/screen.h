#pragma once

#include "platform.h"

class screen {
public:
	bool focused;

	virtual void update() {}
	virtual void draw() {}
	virtual void redraw() {}

	virtual int routine() { return -1; }
};