#pragma once

#include "screen.h"

class screen_briefing : public screen {
public:
	screen_briefing();
	virtual void load(int level) override;
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
	int level;
};