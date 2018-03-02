#pragma once

#include "screen.h"

class screen_test : public screen {
public:
	screen_test();
	virtual void load(int level) override;
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
	int state;
	int level;
};