#pragma once

#include <fxcg/display.h>

inline void BdispH_AreaFill(int x1, int x2, int y1, int y2, unsigned short color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += LCD_WIDTH_PX*y1 + x1;
	for (int j = y1; j <= y2; j++)
	{
		for (int i = x1; i <= x2; i++)
		{
			*(VRAM++) = color;
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

inline void BdispH_SetPoint(int x, int y, unsigned short color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	*(VRAM + LCD_WIDTH_PX*y + x) = color;
}

void Draw_SmallNum(int size, int x, int y, int num, int space, color_t fill_color = COLOR_BLACK, color_t back_color = COLOR_WHITE);
