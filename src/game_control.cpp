#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>
#include "keys.h"
#include "sprites.h"
#include "platform.h"
#include "display_helper.h"
#include "game_control.h"
#include "utils.h"

#include "screen_battle.h"
#include "screen_briefing.h"
#include "screen_upgrade.h"
#include "screen_dialog.h"

game_control gc;

// draw entire screen after pausing/menu/etc.
void game_control::redraw()
{
	Bdisp_AllClr_VRAM();
	active_screen->redraw();
}

void game_control::pause()
{
	display_fill area;
	area.x1 = 80;
	area.x2 = 304;
	area.y1 = 84;
	area.y2 = 130;
	area.mode = 1;
	display_fill temp = area;
	Bdisp_AreaClr(&temp, 1, COLOR_BLACK);

	area.x1 += 2;
	area.x2 -= 2;
	area.y1 += 2;
	area.y2 -= 2;
	Bdisp_AreaClr(&area, 1, COLOR_WHITE);

	DmaWaitNext();
	PrintCXY(138, 72, "Paused", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	int key;
	do
	{
		GetKey(&key);
	} while (key != KEY_CTRL_EXIT);
	prev_time = RTC_GetTicks();
	Bdisp_AllClr_VRAM();
	redraw();
	Bdisp_EnableColor(1);
}

void game_control::set_active_screen(screen* screen)
{
	active_screen = screen;
	redraw();
}

// start game_control
void game_control::start()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	sran(RTC_GetTicks());

	for (int i = 0; i < 10; i++)
		screens[i] = nullptr;
	screens[0] = new screen_upgrade;
	screens[1] = new screen_briefing;
	screens[2] = new screen_battle;
	screens[3] = new screen_dialog;
	int next_screen = 2;
	active_screen = screens[next_screen];

	Bdisp_EnableColor(1);
	Bdisp_AllClr_VRAM();

	fps = 63;
	fps_count[0] = fps_count[1] = 0;

	prev_time = RTC_GetTicks();
	clock = 0;
	last_draw = 0;

	for (int level = 1; level <= 20; )
	{
		((screen_battle*)screens[2])->load(level);
		redraw();
		next_screen = screens[next_screen]->routine();
		update();
		if (next_screen != -1)
		{
			active_screen = screens[next_screen];
			if (next_screen == 2)
				level++;
		}
	}
	int key;
	for (;;)
		GetKey(&key);
}

int lag = 0;
void game_control::update()
{
	for (;;)
	{
		int now = RTC_GetTicks();
		if (now >= prev_time + 2)
		{
			updateKeys();
			if (keys.ac) pause();
			if (keys.menu) pause();
			clock += 2;
			prev_time += 2;
			fps++;

			active_screen->update();

			last_draw++;
			if (last_draw >= 5)
			{
				lag += now - prev_time;
				fps += (now - prev_time) / 2;
				prev_time = now;
			}
			if (now <= prev_time + 1)
			{
				last_draw = 0;
				draw();
			}
			break;
		}
		else if (now < prev_time)
			prev_time = now;
#if TARGET_WINSIM
		Sleep(15);
#else
		CMT_Delay_micros(100);
#endif
	}
}

void game_control::draw()
{
	active_screen->draw();

	fps_count[0]++;
	if (fps >= 32)
	{
		BdispH_AreaFill(300, 383, 0, 23, COLOR_WHITE);
		fps -= 32;
		char buffer[20];
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "fps=%d", fps_count[0] + fps_count[1]);
		fps_count[1] = fps_count[0];
		fps_count[0] = 0;
		int x = 0;
		int y = 0;
		PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
		x = 382 - x;
		y = 1;
		PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
	}

	DmaWaitNext();
	ShowDisplay();
}