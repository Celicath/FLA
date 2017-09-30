#include <fxcg/display.h>
#include "screen_dialog.h"
#include "string.h"
#include "game_control.h"
#include "keys.h"

void screen_dialog::say(screen* parent_, const char* line)
{
	parent = parent_;
	for (int i = 0; ; i++)
	{
		text[i] = line[i];
		if (!line[i])
			break;
	}
	reset_cursor();
	pos = pos_drawn = 0;
	going = true;
}

void screen_dialog::update()
{
	if (going)
	{
		pos++;
		if (!text[pos])
		{
			going = false;
			return;
		}
	}
	if (keys.action)
	{
		pos = 0;
		gc.set_active_screen(parent);
	}
}

void screen_dialog::draw()
{
	for (; pos_drawn < pos; pos_drawn++)
	{
		char k[2];
		k[0] = text[pos_drawn];
		k[1] = 0;
		PrintMini(&x, &y, k, 0x2, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
		if (x > 370)
		{
			x = 4;
			y += 17;
		}
	}
}

void screen_dialog::redraw()
{
	parent->redraw();
	reset_cursor();
	pos_drawn = 0;
	draw();
}