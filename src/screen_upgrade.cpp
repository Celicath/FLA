#include "screen_upgrade.h"
#include "display_helper.h"
#include "keys.h"
#include "sprites.h"
#include "game_control.h"
#include "screen_dialog.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define HPBAR_SIZE 50

const char upgrade_names[][12] =
{
	"Level UP!",
	"Level UP!",
	"Level UP!",
	"Fireball",
	"Frostbolt",
	"Whirlwind",
	"Trap"
};

const char upgrade_descs[][100] =
{
	"HP+2, Atk+1",
	"HP+2, Def+1",
	"HP+2, Spd+1",
	"Deals 6 damage to small area.",
	"Deals 4 damage to small area and slows them.",
	"Deals 1-6 damage to all enemies and push them to right.",
	"Deals 6 damage when activated. Effective against stacked enemies."
};

const int upgrade_types[] =
{
	0, 0, 0, 1, 2, 3, 4
};

const color_t type_colors[] =
{
	COLOR_WHITE, 0xFE18, 0xE71F, 0xC7F8, 0xFFF0
};

upgrades::upgrades(int no_) : no(no_), selected(false) { }

void upgrades::draw(int x, int y)
{
	const char* name = upgrade_names[no];
	const char* desc = upgrade_descs[no];
	int type = upgrade_types[no];

	int width = 80;
	int height = 100;

	BdispH_AreaFill(x, x + width, y, y + height, COLOR_BLACK);
	BdispH_AreaFill(x + 1, x + width - 1, y + 1, y + height - 1, type_colors[type]);
	if (selected)
	{
		BdispH_AreaFill(x + 2, x + width - 2, y + 2, y + height - 2, COLOR_BLACK);
		BdispH_AreaFill(x + 3, x + width - 3, y + 3, y + height - 3, type_colors[type]);
	}

	int s = 0;
	int t = 0;
	PrintMini(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
	s = x + (width - s) / 2;
	t = y + 5;
	PrintMini(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
}

screen_upgrade::screen_upgrade()
{
	load();
}

void screen_upgrade::load()
{
	for (int i = 0; i < 10; i++)
		ups[i] = rand() % 6;
}

void screen_upgrade::update()
{
}

void screen_upgrade::draw()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	if (need_update_ups)
	{
		need_update_ups = false;
		redraw();
	}
}


void screen_upgrade::redraw()
{
	int s = 10;
	int t = 30;
	PrintMini(&s, &t, "Choose upgrade", 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	for (int i = 0; i < 3; i++)
		ups[i].draw(100 * i + 15, 60);

	const char* name = upgrade_names[ups[selected_no].no];
	const char* desc = upgrade_descs[ups[selected_no].no];

	BdispH_AreaFill(10, 383, 170, 210, COLOR_WHITE);

	s = 10;
	t = 170;
	PrintMini(&s, &t, name, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
	s = 15;
	t = 185;
	PrintMini(&s, &t, desc, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
}

int screen_upgrade::routine()
{
	state = 0;
	for (;;)
	{
		gc.update();
		switch (state)
		{
		case 0:
			int diff = keys.right - keys.left;
			if (diff)
			{
				ups[selected_no].selected = false;
				selected_no = (selected_no + diff + 3) % 3;
				ups[selected_no].selected = true;
				need_update_ups = true;
			}

			if (keys.action)
			{
				// do some actions
				state = 1;
			}
		}
	}
	state = 0;
	DmaWaitNext();
	MsgBoxPush(4);
	PrintCXY(80, 42, "Wait for next stage", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	PrintCXY(84, 90, "Press [EXIT]", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	int key;
	do
	{
		GetKey(&key);
	} while (key != KEY_CTRL_EXIT);
	MsgBoxPop();
	gc.prev_time = RTC_GetTicks();
	return 0;
}
