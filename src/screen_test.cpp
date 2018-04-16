#include "screen_test.h"
#include "game_control.h"
#include "player.h"
#include "character.h"
#include "display_helper.h"

screen_test::screen_test()
{
}

const char enemies[][20]
{
	"Shloom",
	"Red Shloom",
	"Blue Shloom",
	"Big Shloom"
};

void screen_test::load(int level_)
{
	level = level_;
	switch (level)
	{
	case 1:
		break;
	}
}

void screen_test::update()
{
}

void screen_test::draw()
{
	redraw();
}

void screen_test::redraw()
{
	Bdisp_AllClr_VRAM();
	char buffer[2];
	buffer[1] = 0;
	for (int i = 0; i < 10; i++)
	{
		int x = 1;
		int y = 24 + i * 12;
		BdispH_AreaFill(0, 0, y, y + 11, COLOR_BLACK);
		for (int j = 0; j < 26; j++)
		{
			if (26 * i + j >= 256) goto hell;
			buffer[0] = 26 * i + j;
			//PrintMiniMini(&x, &y, buffer, 0x50, state, 0);
			PrintMini(&x, &y, buffer, 0x40 + state, 0xffffffff, 0, 0, COLOR_BLUE, COLOR_YELLOWGREEN, 1, 0);
			BdispH_AreaFill(x, x, y, y + 11, COLOR_BLACK);
			x++;
		}
	}
	hell:

	player::pl.show_stats();
}

int screen_test::routine()
{
	int key;
	for (state = 0; state < 8; state++)
	do
	{
		player::pl.set_character(character::bchs[0]);
		gc.update(true);
		DmaWaitNext();

		GetKey(&key);
	} while (key != KEY_CTRL_EXE);

	return 2;
}