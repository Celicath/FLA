
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

#define HPBAR_SIZE 50

const char upgrade_name[][16] =
{
	"Level UP (Atk)",
	"Level UP (Def)",
	"Level UP (Spd)",
	"Gain Strength",
	"Potion",
	"Fireball",
	"Ice Lance",
	"Wind Blast",
	"Earthquake"
};

/* 0=passive 1=normal 2=fire 3=ice 4=wind 5=earth */
const int upgrade_type[] =
{
	1, 1, 0, 0, 0, 2, 3, 4, 5
};

const char type_name[][16] =
{
	"Passive",
	"Spell",
	"Spell(Fire)",
	"Spell(Ice)",
	"Spell(Wind)",
	"Spell(Earth)",
};

const color_t type_colors[] = { COLOR_WHITE, 0xFE18, 0xE71F, 0xC7F8, 0xFFF0 };
//const color_t type_colors2[] = { COLOR_WHITE, 0xF800, 0x001F, 0x0600, 0xC600 };

upgrade::upgrade(int no_) : no(no_), selected(false) { }

void upgrade::draw(int x, int y)
{
	CopySprite(sprite_icons[no], x, y, 32, 32);
	if (selected)
		draw_target_border(x, y, 32, 32, COLOR_BLACK);
}

void upgrade::get_description(char* buffer, int no)
{
	memset(buffer, 0, sizeof(buffer));
	switch(no)
	{
	case 0: case 1: case 2:
		sprintf(buffer, "Increase %s by 1.", no == 0 ? "Attack" : no == 1 ? "Deffense" : "Speed");
		break;
	case 3: sprintf(buffer, "Increase your attack by 1 this turn."); break;
	case 4: sprintf(buffer, "Restore 10 HP."); break;
	case 5:	sprintf(buffer, "6 damage to an enemy."); break;
	case 6: sprintf(buffer, "4 damage + slow to an enemy."); break;
	case 7: sprintf(buffer, "3 damage + knockback to all enemies."); break;
	case 8: sprintf(buffer, "4 damage to Area(large)."); break;
	}
}

screen_upgrade::screen_upgrade() : num_options(4)
{
}

void screen_upgrade::shuffle()
{
	for (int i = 0; i < num_options; i++)
	{
		int k = ran() % 7 + 2;
		if (k < 5) k -= 2;
		ups[i] = upgrade(k);
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
	int no = ups[selected_no].no;
	const char* name = upgrade_name[no];

	int s = 20;
	int t = 135;

	PrintMini(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	s = 26;
	t = 160;
	PrintMiniMini(&s, &t, type_name[upgrade_type[no]], 0x50, TEXT_COLOR_BLACK, 0);

	upgrade::get_description(buffer, no);

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
				player::pl.upgrades[ups[selected_no].no]++;
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
