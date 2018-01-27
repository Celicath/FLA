#pragma once

#include "screen.h"

#define HPBAR_SIZE 50

class character
{
public:
	const color_t* image;
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

	int wait_gauge;
	int rotation;

	character() : target_mode(0) { };
	character(int no);
	void draw_target_border(color_t color);
	void set_image(const color_t* image_);

	static character bchs[10];
private:
	void set_stat(const color_t* image_, int width_, int height_, int mode_, int hp_, int attack_, int defense_, int speed_);
};
