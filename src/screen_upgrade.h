#pragma once

#include "screen.h"
#include "upgrade.h"

class screen_upgrade : public screen {
public:
	screen_upgrade();
	void shuffle();
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
	void draw_desc();
	int selected_no;

	int need_redraw;

	int rest;
	int num_options;
	upgrade ups[8];
};
