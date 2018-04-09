#pragma once

#include "screen.h"

class upgrade
{
public:
	int no;
	const color_t* image;
	bool selected;

	upgrade() { }
	upgrade(int no_);

	void draw(int x, int y);

	static void get_description(char* buffer, int no);
};


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

	bool need_redraw;

	int rest;
	int num_options;
	upgrade ups[10];
};

extern const char upgrade_name[][16];