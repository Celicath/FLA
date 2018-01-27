#pragma once

#include "screen.h"

class screen_stat : public screen {
public:
	screen_stat();
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
};