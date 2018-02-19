#include "display_helper.h"

void draw_small_num(int size, int x, int y, int num, int space, color_t fill_color, color_t back_color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	int temp[20];
	int d = 0;

	int pos = x + y * LCD_WIDTH_PX;
	if (num < 0)
	{
		for (int i = 0; i < 3 * size; i++)
			for (int j = 0; j < size; i++)
				VRAM[pos + i + (4 + j) * LCD_WIDTH_PX] = fill_color;
		pos += size * 4;
		num = -num;
	}
	do
	{
		temp[d++] = num % 10;
		num /= 10;
	} while (num > 0 || d < space);

	while (d > 0)
	{
		int k = temp[--d];
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
			{
				VRAM[pos + i + j * LCD_WIDTH_PX] =
					(k != 1 ? fill_color : back_color);
				VRAM[pos + i + (j + size) * LCD_WIDTH_PX] =
					((k == 0 || k >= 4) ? fill_color : back_color);
				VRAM[pos + i + (j + 2 * size) * LCD_WIDTH_PX] =
					VRAM[pos];
				VRAM[pos + i + (j + 3 * size) * LCD_WIDTH_PX] =
					(((k & 1) == 0 && k != 4) ? fill_color : back_color);
				VRAM[pos + i + (j + 4 * size) * LCD_WIDTH_PX] =
					((k % 3 != 1) ? fill_color : back_color);

				VRAM[pos + i + size + j * LCD_WIDTH_PX] =
					((k != 1 && k != 4) ? fill_color : back_color);
				VRAM[pos + i + size + (j + 2 * size) * LCD_WIDTH_PX] =
					((k >= 2 && k != 7) ? fill_color : back_color);
				VRAM[pos + i + size + (j + 4 * size) * LCD_WIDTH_PX] =
					VRAM[pos + i + (j + 4 * size) * LCD_WIDTH_PX];

				VRAM[pos + i + 2 * size + j * LCD_WIDTH_PX] =
					fill_color;
				VRAM[pos + i + 2 * size + (j + size) * LCD_WIDTH_PX] =
					((k <= 4 || k >= 7) ? fill_color : back_color);
				VRAM[pos + i + 2 * size + (j + 2 * size) * LCD_WIDTH_PX] =
					fill_color;
				VRAM[pos + i + 2 * size + (j + 3 * size) * LCD_WIDTH_PX] =
					((k != 2) ? fill_color : back_color);
				VRAM[pos + i + 2 * size + (j + 4 * size) * LCD_WIDTH_PX] =
					fill_color;
			}
		pos += 4 * size;
	}
}

void border_helper(int x, int y, int dx, int dy, color_t color)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	for (int i = 0; i < 20; i++)
	{
		VRAM[y * LCD_WIDTH_PX + x] = color;
		if (i < 5) x += dx;
		else if (i < 7) y += dy;
		else if (i < 10) x -= dx;
		else if (i < 13) y += dy;
		else if (i < 15) x -= dx;
		else y -= dy;
	}
}

void draw_target_border(int x, int y, int width, int height, color_t color)
{
	border_helper(x - width / 2 - 3, y - height / 2 - 3, 1, 1, color);
	border_helper(x + width - width / 2 + 2, y - height / 2 - 3, -1, 1, color);
	border_helper(x - width / 2 - 3, y + height - height / 2 + 2, 1, -1, color);
	border_helper(x + width - width / 2 + 2, y + height - height / 2 + 2, -1, -1, color);
}