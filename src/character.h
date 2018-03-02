#pragma once

#include "screen.h"

#define HPBAR_INTERVALS 50

class character
{
public:
	const color_t* image;
	int no;
	int x;
	int y;
	int prev_x;
	int prev_y;
	int need_update;
	int width;
	int height;
	int mode;

	int level;
	int hp;
	int mhp;
	int attack;
	int defense;
	int speed;
	int target_mode;	// 0=no border 1=border -1=erase border

	int prev;
	int next;

	int hpbar;
	int hpbar_duration;
	int death_animation;
	int rotation;

	int target_pos;

	character() : target_mode(0) { };
	character(int no);
	void draw_target_border(color_t color, bool draw_name = true);
	void set_image(const color_t* image_);
	const char* get_name() const;

	static character bchs[10];
private:
	void set_stat(const color_t* image_, int no_, int width_, int height_, int mode_, int hp_, int attack_, int defense_, int speed_);
};
