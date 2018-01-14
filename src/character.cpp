#include "character.h"
#include "sprites.h"

character::character(int no)
	: target_mode(0)
{
	switch (no)
	{
	case -1:
		set_stat(sprite_flipp[0], 32, 32, 0, 20, 4, 0, 2);
		break;
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

void border_helper(int x, int y, int dx, int dy, color_t color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	for (int i = 0; i < 20; i++)
	{
		VRAM[y * LCD_WIDTH_PX + x] = color;
		if (i < 5) x += dx;
		else if (i < 7) y += dy;
		else if (i < 10) x -= dx;
		else if (i < 13) y += dy;
		else if (i < 15) x -= dx;
		else y -= dy;
	}
}

void character::draw_target_border(color_t color)
{
	border_helper(x - width / 2 - 3, y - height / 2 - 3, 1, 1, color);
	border_helper(x + width - width / 2 + 2, y - height / 2 - 3, -1, 1, color);
	border_helper(x - width / 2 - 3, y + height - height / 2 + 3, 1, -1, color);
	border_helper(x + width - width / 2 + 2, y + height - height / 2 + 3, -1, -1, color);
}
