#include "effect.h"
#include "fxcg/display.h"
#include <string.h>

effect::effect(const char* text_, int x, int y, int frame, int color)
	: x(x), y(y), frame(frame), prev_frame(frame), color(color)
{
	bool zero = false;
	for (int i = 0; i < 16; i++)
	{
		if (text_[i] == 0) zero = true;
		text[i] = zero ? 0 : text_[i];
	}
}

void effect::draw(int draw_mode)
{
	if (text[0])
	{
		if (draw_mode && frame < 0)
			text[0] = 0;
		else
		{
			int sx = 0;
			int sy = y - 20 + (draw_mode ? frame * frame : prev_frame * prev_frame) / 100;
			PrintMiniMini(&sx, &sy, text, 0x52, draw_mode ? color : TEXT_COLOR_WHITE, 1);
			sx = x - sx / 2;
			PrintMiniMini(&sx, &sy, text, 0x52, draw_mode ? color : TEXT_COLOR_WHITE, 0);
			prev_frame = frame;
		}
	}
}
