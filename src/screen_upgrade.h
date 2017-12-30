#pragma once

#include "screen.h"

class upgrades
{
public:
	int no;
	const color_t* image;
	bool selected;

	upgrades() { }
	upgrades(int no_);

	void draw(int x, int y);
};


class screen_upgrade : public screen {
public:
	screen_upgrade();
	void load();
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
	int selected_no;
	int state;

	bool need_update_ups;

	upgrades ups[10];
};