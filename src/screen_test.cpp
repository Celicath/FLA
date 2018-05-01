#include "screen_test.h"
#include "game_control.h"
#include "player.h"
#include "character.h"
#include "display_helper.h"
#include "keys.h"
#include "utils.h"

#include <math.h>

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
}

void screen_test::update()
{
}

void screen_test::draw()
{
	/*
	color_t* VRAM = (color_t*)GetVRAMAddress();

	int sx = 100;
	int sy = 100;
	int radius = 160;
	int inner_radius = 50;
	int outer_radius = 120;
	for (int i = -12; i <= 12; i++)
		for (int j = -12; j <= 12; j++)
		{
			double c = cos(pos * 0.05);
			double s = sin(pos * 0.05);
			double x = i * c + j * s;
			double y = i * s - j * c;

			BdispH_SetPoint(sx + i, sy + j, 0x0000);
			if (x * x + y * y * 6 < inner_radius)
				BdispH_SetPoint(sx + i, sy + j, 0xFFFF);
			else if (i * i + j * j <= radius)
			{
				int k1 = (int)(Q_rsqrt(1 / (1 - (double)(i * i + j * j) / radius)) * 32);
				int k2 = 32 - (i * i + j * j) * 32 / radius;
				BdispH_SetPointAlpha(sx + i, sy + j, (0x87EF - 0x0801 * (k2 / 5)), k1);
			}
			if (x * x * 20 + y * y <= outer_radius + 32)
				BdispH_SetPointAlpha(sx + i, sy + j, 0xFFFF, 32 - abs((int)((x * x * 20 + y * y + 1 - outer_radius) * 32 / outer_radius)));
		}

	player::pl.show_stats();
	*/
	redraw();
}

void screen_test::redraw()
{
	/*
	Bdisp_AllClr_VRAM();
//	BdispH_AreaFill(0, 383, 0, 191, COLOR_BLACK);
	draw();
	*/

	Bdisp_AllClr_VRAM();

	char buffer[4];
	buffer[1] = buffer[2] = buffer[3] = 0;
	for (int i = 0; i < 10; i++)
	{
		int x = 1;
		int y = 24 + i * 16;

		BdispH_AreaFill(0, 0, y, y + 11, COLOR_BLACK);
		for (int j = 0; j < 26; j++)
		{
			if (26 * i + j >= 128) goto hell;
			buffer[0] = 26 * i + j;
			//PrintMiniMini(&x, &y, buffer, 0x50, state, 0);
			//PrintMini(&x, &y, buffer, 0x40 + state, 0xffffffff, 0, 0, COLOR_BLUE, COLOR_YELLOWGREEN, 1, 0);
			Bdisp_MMPrintRef(&x, &y, buffer, 0x40 + state, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
			BdispH_AreaFill(x, x, y, y + 15, COLOR_BLACK);
			x++;
		}
	}
hell:;
	//player::pl.show_stats();
}

int screen_test::routine()
{
	int key;
	for (state = 0; state < 8; state++)
	do
	{
		//player::pl.set_character(character::bchs[0]);
		gc.update(true);
		DmaWaitNext();

		GetKey(&key);
	} while (key != KEY_CTRL_EXE);

	return 1;
}
