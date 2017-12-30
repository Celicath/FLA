#pragma once

#include "screen.h"

class map_character
{
public:
	const color_t* image;
	int x;
	int y;
	int prev_x;
	int prev_y;
	int width;
	int height;
	int mode;
	map_character() {}
	map_character(const color_t* image_, int x_, int y_, int width_, int height_, int mode_);
};

class screen_map : public screen {
public:
	void load(int loc);
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
private:
	void char_draw(map_character& ch, bool flip);
	inline color_t get_pixel(int x, int y);
	const static int minx[];
	const static int maxx[];
	const static int miny[];
	const static int maxy[];
	int loc;
	int x;
	int y;
	int x_prev;
	int y_prev;

	int dia_count;
};