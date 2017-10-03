#pragma once

#include "screen.h"

class screen_dialog : public screen {
public:
	char text[256];

	void say(screen* parent_, const char* line);
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;

private:
	inline void reset_cursor() { x = 4; y = 24; }
	int x;
	int y;
	int pos;
	int pos_drawn;
	bool going;
	bool padding[3];

	screen* parent;
};