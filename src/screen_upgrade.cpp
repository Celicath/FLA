
#include "screen_upgrade.h"
#include "display_helper.h"
#include "keys.h"
#include "sprites.h"
#include "game_control.h"
#include "screen_dialog.h"
#include "utils.h"
#include "player.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

screen_upgrade::screen_upgrade() : num_options(4)
{
}

void screen_upgrade::shuffle()
{
	for (int i = 0; i < num_options; i++)
	{
		int k = ran() % 4 + 5;
		if (k < 5) k -= 2;
		ups[i] = upgrade(upgrade::SPELL, k);
		for (int j = 0; j < i; j++)
			if (ups[i].no == ups[j].no)
			{
				i--;
				break;
			}
	}
}

void screen_upgrade::update()
{
}

void screen_upgrade::draw()
{
	if (need_redraw)
	{
		need_redraw = false;
		redraw();
		player::pl.show_stats();
	}
}

char buffer[60];

void screen_upgrade::redraw()
{
	BdispH_AreaFill(0, LCD_WIDTH_PX - 1, 16, LCD_HEIGHT_PX - 1, COLOR_WHITE);
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "- Choose bonus: %d -", player::pl.rest);
	int s = 0;
	int t = 40;
	PrintMini(&s, &t, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
	s = 191 - s / 2;
	PrintMini(&s, &t, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	for (int i = 0; i < num_options; i++)
		ups[i].draw(191 - 24 * (num_options - 1) + 48 * i, 90);

	draw_desc();
}

void screen_upgrade::draw_desc()
{
	const char* name = ups[selected_no].get_name();

	int s = 20;
	int t = 135;

	Bdisp_MMPrintRef(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	s = 26;
	t = 160;
	PrintMiniMini(&s, &t, ups[selected_no].get_type(), 0x50, TEXT_COLOR_BLACK, 0);

	ups[selected_no].get_description(buffer);

	s = 26;
	t = 175;
	PrintMiniMini(&s, &t, buffer, 0x40, TEXT_COLOR_BLACK, 0);
}

int screen_upgrade::routine()
{
	player::pl.rest++;

	while (player::pl.rest)
	{
		shuffle();
		need_redraw = true;
		selected_no = 0;
		for (int i = 0; i < num_options; i++)
			ups[i].selected = (i == 0);

		for (;;)
		{
			gc.update();
			int diff = keys.right - keys.left;
			if (diff)
			{
				ups[selected_no].selected = false;
				selected_no = (selected_no + diff + num_options) % num_options;
				ups[selected_no].selected = true;
				need_redraw = true;
			}

			if (keys.action)
			{
				if (ups[selected_no].type == upgrade::SPELL)
					player::pl.spells[ups[selected_no].no]++;
				else player::pl.passives[ups[selected_no].no]++;
				player::pl.rest--;
				player::pl.set_deck();
				player::pl.set_character(character::bchs[0]);
				break;
			}
		}
	}
	for (int i = 0; i <= 2; i++)
		gc.update(i == 2);
	return 1;
}
