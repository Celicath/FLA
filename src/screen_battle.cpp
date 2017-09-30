#include "screen_battle.h"
#include "display_helper.h"
#include "keys.h"
#include "sprites.h"
#include "game_control.h"
#include "screen_dialog.h"

battle_character::battle_character(int no)
	: targeting(false)
{
	switch (no)
	{
	case -1:
		set_stat(sprite_flipp[0], 32, 32, 0, 10, 4, 0, 2, 1);
		break;
	case 0:
		set_stat(sprite_enemy1, 23, 25, 1, 10, 1, 0, 1, 0);
		break;
	}
}

void battle_character::set_stat(const color_t* image_, int width_, int height_, int mode_, int hp_, int attack_, int defense_, int speed_, int luck_)
{
	image = image_;
	width = width_;
	height = height_;
	mode = mode_;
	hp = hp_;
	attack = attack_;
	defense = defense_;
	speed = speed_;
	luck = luck_;
}

void battle_character::set_targeting_false()
{
	targeting = false;
	draw_target_border(COLOR_WHITE);
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

void battle_character::draw_target_border(color_t color)
{
	border_helper(x - 3, y - 3, 1, 1, color);
	border_helper(x + width + 2, y - 3, -1, 1, color);
	border_helper(x - 3, y + height + 2, 1, -1, color);
	border_helper(x + width + 2, y + height + 2, -1, -1, color);
}

screen_battle::screen_battle()
{
	load(0);
}

void screen_battle::load(int troop_no)
{
	command_no = 0;

	bchs[0] = battle_character(-1);
	bchs[0].x = 80;
	bchs[0].y = 120;
	switch (troop_no)
	{
	case 0:
		bchs[1] = battle_character(0);
		bchs[2] = battle_character(0);
		break;
	}
	for (int i = 1; i < 10; i++)
	{
		bchs[i].x = 240 + 40 * i;
		bchs[i].y = 120;
	}
}

void screen_battle::update()
{
}

void screen_battle::draw()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	draw_icons(false);

	for (int i = 0; i < 10; i++)
	{
		battle_character& b = bchs[i];
		if (b.image != nullptr && (b.prev_x != b.x || b.prev_y != b.y))
		{
			for (int j = b.prev_y; j < b.prev_y + b.height; j++)
			{
				for (int i = b.prev_x; i < b.prev_x + b.width; i++)
				{
					if (i >= 0 && j >= 24 && i < 384 && j < 216)
						VRAM[j * LCD_WIDTH_PX + i] = 0xffff;
				}
			}
		}
	}
	for (int i = 0; i < 10; i++)
	{
		battle_character& b = bchs[i];
		if (b.image != nullptr)
		{
			CopySprite(b.image, b.x, b.y, b.width, b.height, false, b.mode);
			if (b.targeting)
				b.draw_target_border(COLOR_BLACK);
		}
		b.prev_x = b.x;
		b.prev_y = b.y;
	}
}

const char command_text[][12] =
{
	"Jump",
	"Push",
	"Item",
	"Guard",
	"Tactics",
	"Run"
};

void screen_battle::redraw()
{
	draw_icons(true);
}

int screen_battle::routine()
{
	state = 0;
	for (;;)
	{
		gc.update();
		switch (state)
		{
		case 0:
			if (keys.right && command_no < 5)
			{
				command_no++;
			}
			if (keys.left && command_no > 0)
			{
				command_no--;
			}
			if (keys.action)
			{
				target_no = 1;
				state = 1;
			}
			for (int i = 1; i <= 6; i++)
				if (keys.f_key[i])
				{
					command_no = i - 1;
					target_no = 1;
					state = 1;
				}
			break;
		case 1:
			if (keys.right)
			{
				bchs[target_no].set_targeting_false();
				do
				{
					target_no++;
					if (target_no >= 10) target_no = 1;
				} while (bchs[target_no].image == nullptr);
			}
			if (keys.left)
			{
				bchs[target_no].set_targeting_false();
				do
				{
					target_no--;
					if (target_no < 1) target_no = 9;
				} while (bchs[target_no].image == nullptr);
			}
			bchs[target_no].targeting = true;

			if (keys.action)
			{
				bchs[target_no].set_targeting_false();
				state = 2;
				attack(target_no);
				state = 0;
			}
			else if (keys.cancel)
				state = 0;
			break;
		}
	}
	return 0;
}

void screen_battle::attack(int target)
{
	int x1 = bchs[0].x;
	int y1 = bchs[0].y;
	int px = bchs[0].x;
	int py = bchs[0].y;
	int nx = bchs[target].x;
	int ny = bchs[target].y - bchs[target].height;

	for (int a = 0; a < 3; a++)
	{
		// 0=Initial -=failed +=success
		int action_command = 0;

		int range = 8 - a;

		for (int f = 0; f < 40 + range / 2; f++)
		{
			if (f <= 40)
			{
				bchs[0].x = (px * (40 - f) + nx * f) / 40;
				bchs[0].y = (py * (40 - f) + ny * f + (f - 20) * (f - 20) * 7 - 2800) / 40;
			}
			else if (action_command != 0) break;

			if (keys.action && action_command >= 0)
			{
				if (f < 40 - (range + 1) / 2)
					action_command = (f < 20 ? 0 : -1);
				else action_command = 1;
			}
			gc.update();
		}
		px = nx;
		py = ny;
		//Damage(target, cur_bch.AP);
		if (action_command != 1)
			break;
	}

	for (int f = 0; f <= 40; f++)
	{
		bchs[0].x = (px * (40 - f) + x1 * f) / 40;
		bchs[0].y = (py * (40 - f) + y1 * f + (f - 20) * (f - 20) * 7 - 2800) / 40;
		gc.update();
	}
}

void screen_battle::draw_icons(bool always_draw)
{
	static int prev_command_no = -1;
	static int prev_state = -1;
	if (!always_draw && prev_command_no == command_no && prev_state == state) return;
	prev_command_no = command_no;
	prev_state = state;

	BdispH_AreaFill(0, 383, 155, 215, COLOR_WHITE);

	int istart = 0;
	int iend = 5;
	if (state == 1) istart = iend = command_no;
	else if (state == 2) iend = -1;
	for (int i = istart; i <= iend; i++)
	{
		BdispH_AreaFill(64 * i + 2, 64 * i + 60, 173, 173, COLOR_BLACK);
		BdispH_AreaFill(64 * i + 1, 64 * i + 1, 173, 214, COLOR_BLACK);

		BdispH_AreaFill(64 * i + 3, 64 * i + 3, 175, 179, COLOR_BLACK);
		BdispH_AreaFill(64 * i + 4, 64 * i + 5, 175, 175, COLOR_BLACK);
		BdispH_AreaFill(64 * i + 4, 64 * i + 5, 177, 177, COLOR_BLACK);
		Draw_SmallNum(1, 64 * i + 7 - !i, 175, i + 1, 1);

		CopySprite(sprite_command[i], 64 * i + 12, 175, 40, 40, false);

		if (command_no == i)
		{
			if (state == 0)
			{
				BdispH_AreaFill(64 * i + 2, 64 * i + 59, 172, 172, COLOR_BLACK);
				BdispH_AreaFill(64 * i + 2, 64 * i + 59, 214, 215, COLOR_BLACK);
				BdispH_AreaFill(64 * i + 0, 64 * i + 0, 174, 213, COLOR_BLACK);
				BdispH_AreaFill(64 * i + 60, 64 * i + 61, 174, 213, COLOR_BLACK);
			}
			else if (state == 1)
				BdispH_AreaReverse(64 * i + 1, 64 * i + 60, 173, 214);

			int x = 64 * i + 32 - strlen(command_text[command_no]) * 6;
			int y = 155;
			PrintMini(&x, &y, command_text[command_no], 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
		}
	}
}
