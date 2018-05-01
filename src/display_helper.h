#pragma once

#include <fxcg/display.h>

inline void BdispH_AreaFill(int x1, int x2, int y1, int y2, int color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += LCD_WIDTH_PX*y1 + x1;
	for (int j = y1; j <= y2; j++)
	{
		for (int i = x1; i <= x2; i++)
		{
			*(VRAM++) = (color_t)color;
		}
		VRAM += LCD_WIDTH_PX - (x2 - x1 + 1);
	}
}

inline void BdispH_AreaReverse(int x1, int x2, int y1, int y2)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += LCD_WIDTH_PX*y1 + x1;
	for (int j = y1; j <= y2; j++)
	{
		for (int i = x1; i <= x2; i++)
		{
			*VRAM = 0xffff - *VRAM;
			VRAM++;
		}
		VRAM += LCD_WIDTH_PX - (x2 - x1 + 1);
	}
}

inline void BdispH_SetPoint(int x, int y, int color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	*(VRAM + LCD_WIDTH_PX*y + x) = (color_t)color;
}

// alpha : 32=opaque, 0=transparent
inline void BdispH_SetPointAlpha(int x, int y, int color, int alpha)
{
	color_t* VRAM = (color_t*)GetVRAMAddress() + LCD_WIDTH_PX*y + x;
	*VRAM = (color_t)(
		((((color >> 11) * alpha + (*VRAM >> 11) * (32 - alpha)) / 32) << 11) +
		(((((color >> 5) & 0x3f) * alpha + ((*VRAM >> 5) & 0x3f) * (32 - alpha)) / 32) << 5) +
		(((color & 0x1f) * alpha + (*VRAM & 0x1f) * (32 - alpha)) / 32));
}

// alpha : 32=opaque, 0=transparent
inline void BdispH_SetPointAlpha(color_t* vram_pos, int color, int alpha)
{
	*vram_pos = (color_t)(
		((((color >> 11) * alpha + (*vram_pos >> 11) * (32 - alpha)) / 32) << 11) +
		(((((color >> 5) & 0x3f) * alpha + ((*vram_pos >> 5) & 0x3f) * (32 - alpha)) / 32) << 5) +
		(((color & 0x1f) * alpha + (*vram_pos & 0x1f) * (32 - alpha)) / 32));
}

void draw_small_num(int size, int x, int y, int num, int space, int fill_color = COLOR_BLACK, int back_color = COLOR_WHITE);
void draw_target_border(int x, int y, int width, int height, int color);