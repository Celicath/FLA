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
	case 1:
		set_stat(sprite_enemy1, no, 23, 25, 0, 10, 3, 0, 1);
		break;
	case 2:
		set_stat(sprite_enemy1, no, 23, 25, 0, 15, 4, 0, 2);
		break;
	case 3:
		set_stat(sprite_enemy1, no, 23, 25, 0, 20, 3, 0, 2);
		break;
	case 4:
		set_stat(sprite_enemy1, no, 23, 25, 1, 30, 3, 0, 4);
		break;
	}
	prev = -1;
	next = -1;
}

void character::set_stat(const color_t* image_, int no_, int width_, int height_, int mode_, int hp_, int attack_, int defense_, int speed_)
{
	image = image_;
	no = no_;
	width = width_;
	height = height_;
	mode = mode_;
	hp = mhp = hp_;
	attack = attack_;
	defense = defense_;
	speed = speed_;
	hpbar = HPBAR_INTERVALS;
	hpbar_duration = 0;
	death_animation = 0;
	rotation = 0;
}

void character::draw_target_border(color_t color, bool draw_name)
{
	::draw_target_border(x, y, width, height, color);
	if (draw_name)
	{
		int sx = 0;
		int sy = y - height / 2 - 14;
		PrintMiniMini(&sx, &sy, get_name(), 0x42, color == COLOR_WHITE ? TEXT_COLOR_WHITE : TEXT_COLOR_BLACK, 1);
		sx = x - sx / 2;
		PrintMiniMini(&sx, &sy, get_name(), 0x42, color == COLOR_WHITE ? TEXT_COLOR_WHITE : TEXT_COLOR_BLACK, 0);
	}
}

void character::set_image(const color_t* image_)
{
	image = image_;
	need_update = true;
}

const char names[][16] =
{
	"Flip",
	"Shloom",
	"Red Shloom",
	"Blue Shloom",
	"King Shloom",
};

const char* character::get_name() const
{
	return names[no];
}
