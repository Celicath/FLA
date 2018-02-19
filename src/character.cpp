#include "character.h"
#include "sprites.h"
#include "player.h"
#include "display_helper.h"

character character::bchs[10];

character::character(int no)
	: target_mode(0)
{
	switch (no)
	{
	case 0:
		set_stat(sprite_enemy1, 23, 25, 0, 10, 2, 0, 1);
		break;
	case 1:
		set_stat(sprite_enemy1, 23, 25, 0, 15, 3, 0, 2);
		break;
	case 2:
		set_stat(sprite_enemy1, 23, 25, 0, 15, 2, 1, 3);
		break;
	case 3:
		set_stat(sprite_enemy1, 23, 25, 1, 30, 3, 1, 2);
		break;
	}
	prev = -1;
	next = -1;
}

void character::set_stat(const color_t* image_, int width_, int height_, int mode_, int hp_, int attack_, int defense_, int speed_)
{
	image = image_;
	width = width_;
	height = height_;
	mode = mode_;
	hp = mhp = hp_;
	attack = attack_;
	defense = defense_;
	speed = speed_;
	hpbar = HPBAR_SIZE;
	hpbar_duration = 0;
	death_animation = 0;
	rotation = 0;
}

void character::draw_target_border(color_t color)
{
	::draw_target_border(x, y, width, height, color);
}

void character::set_image(const color_t* image_)
{
	image = image_;
	need_update = true;
}
