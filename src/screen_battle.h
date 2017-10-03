#pragma once

#include "screen.h"

class battle_character
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
	int hp;
	int mhp;
	int attack;
	int defense;
	int speed;
	int luck;
	bool targeting;
	bool padding[3];

	battle_character() : targeting(false) { };
	battle_character(int no);
	void set_targeting_false();
	void draw_target_border(color_t color);
private:
	void set_stat(const color_t* image_, int width_, int height_, int mode_, int hp_, int attack_, int defense_, int speed_, int luck_);
};


class screen_battle : public screen {
public:
	screen_battle();
	char text[256];
	void load(int troop_no);
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
	int command_no;
	int target_no;
	int state;
	void draw_icons(bool draw_icons);
	void add_effect(int type, int x, int y);

	void attack(int target);
	void damage(int target, int damage, int message);

	battle_character bchs[10];	// 0=flipp, 1~9=enemies
	int effects[10][5];	// 0=type 1=x 2=y 3=count 4=prev_count
};