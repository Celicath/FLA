
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
	"Fireball",
	"Frostbolt",
	"Strong Wind",
	"Ground Trap"
};

/* 5th digit: 0=passive 1=card
   1st digit: 0=normal 1=fire 2=ice 3=wind 4=earth */
const int upgrade_type[] =
{
	0, 0, 0, 6, 7, 8, 9
};

const char type_name[][16] =
{
	"Passive",
	"Passive, Fire",
	"Passive, Ice",
	"Passive, Wind",
	"Passive, Earth",
	"Card",
	"Card, Fire",
	"Card, Ice",
	"Card, Wind",
	"Card, Earth",
};

const color_t type_colors[] = { COLOR_WHITE, 0xFE18, 0xE71F, 0xC7F8, 0xFFF0 };
//const color_t type_colors2[] = { COLOR_WHITE, 0xF800, 0x001F, 0x0600, 0xC600 };

upgrade::upgrade(int no_) : no(no_), selected(false) { }

void upgrade::draw(int x, int y)
{
	const char* name = upgrade_name[no];
	int type = upgrade_type[no] % 5;

	int width = 200;
	int height = 20;

	if (selected)
	{
		BdispH_AreaFill(x - 2, x + width + 2, y - 2, y + height + 2, COLOR_BLACK);
		BdispH_AreaFill(x - 1, x + width + 1, y - 1, y + height + 1, COLOR_WHITE);
	}
	else BdispH_AreaFill(x - 2, x + width + 2, y - 2, y + height + 2, COLOR_WHITE);

	BdispH_AreaFill(x, x + width, y, y + height, COLOR_BLACK);
	BdispH_AreaFill(x + 1, x + width - 1, y + 1, y + height - 1, type_colors[type]);

	int s = x + 2;
	int t = y + 2;
	PrintMini(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
}

screen_upgrade::screen_upgrade()
{
}

void screen_upgrade::shuffle()
{
	for (int i = 0; i < 7; i++)
	{
		ups[i] = upgrade(ran() % 7);
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
	}
}


char buffer[60];

void screen_upgrade::redraw()
{
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "- Choose bonus: %d -", player::pl->rest);
	int s = 10;
	int t = 10;
	BdispH_AreaFill(s, 200, t, t+20, COLOR_WHITE);
	PrintMini(&s, &t, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	for (int i = 0; i < 4; i++)
		ups[i].draw(20, 38 + 30 * i);

	draw_desc();
}

void screen_upgrade::draw_desc()
{
	int no = ups[selected_no].no;
	const char* name = upgrade_name[no];

	int s = 10;
	int t = 160;
	BdispH_AreaFill(s, 383, t, 215, COLOR_WHITE);

	PrintMini(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	s = 15;
	t = 183;
	PrintMiniMini(&s, &t, type_name[upgrade_type[no]], 0x50, TEXT_COLOR_BLACK, 0);

	memset(buffer, 0, sizeof(buffer));
	if (no < 3)
		sprintf(buffer, "HP+%d, %s+1", 2, no == 0 ? "Atk" : no == 1 ? "Def" : "Spd");
	else if (no == 3)
		sprintf(buffer, "6 damage to Area(small).");
	else if (no == 4)
		sprintf(buffer, "4 damage + slow to Area(small).");
	else if (no == 5)
		sprintf(buffer, "3 damage + knockback to all enemies.");
	else if (no == 6)
		sprintf(buffer, "Sets up a 6 damage trap. 2x damage to stacked enemies.");
	s = 15;
	t = 196;
	PrintMiniMini(&s, &t, buffer, 0x40, TEXT_COLOR_BLACK, 0);
}

int screen_upgrade::routine()
{
	player::pl->rest++;

	while (player::pl->rest)
	{
		shuffle();
		need_redraw = true;
		selected_no = 0;
		for (int i = 0; i < 4; i++)
			ups[i].selected = (i == 0);

		for (;;)
		{
			gc.update();
			int diff = keys.down - keys.up;
			if (diff)
			{
				ups[selected_no].selected = false;
				selected_no = (selected_no + diff + 4) % 4;
				ups[selected_no].selected = true;
				need_redraw = true;
			}

			if (keys.action)
			{
				player::pl->upgrades[ups[selected_no].no]++;
				player::pl->rest--;
				break;
			}
		}
	}
	DmaWaitNext();
	int key;
	do
	{
		GetKey(&key);
	} while (key != KEY_CTRL_EXE);
	return 1;
}
