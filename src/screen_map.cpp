#include "screen_map.h"
#include "display_helper.h"
#include "keys.h"
#include "sprites.h"
#include "game_control.h"
#include "screen_dialog.h"

map_character::map_character(const color_t* image_, int x_, int y_, int width_, int height_, int mode_)
	: image(image_), x(x_), y(y_), width(width_), height(height_), mode(mode_) { }

map_character flipp;	// player character
map_character npcs[20];

const int screen_map::minx[] = { 0, 0, 0 };
const int screen_map::maxx[] = { 0, 0, 0 };
const int screen_map::miny[] = { 0, 0, 0 };
const int screen_map::maxy[] = { 0, 0, 0 };

int animation = 0;
int animation_cycle = 2;

void screen_map::load(int loc_)
{
	loc = loc_;
	for (int i = 0; i < 20; i++)
		npcs[i].image = nullptr;
	if (loc == 1)
		npcs[0] = map_character(sprite_enemy1, 80, 80, 23, 25, 1);
	flipp = map_character(sprite_flipp[3], 0, 0, 32, 32, 0);
	dia_count = 0;
}

void screen_map::update()
{
	int dx = 2 * (keys.right_hold - keys.left_hold);
	int dy = 2 * (keys.down_hold - keys.up_hold);

	int nx = flipp.x + dx;
	int ny = flipp.y + dy;
	if (nx < minx[loc] - 176) nx = minx[loc] - 176;
	if (nx > maxx[loc] + 176) nx = maxx[loc] + 176;
	if (ny < miny[loc] - 80) ny = miny[loc] - 80;
	if (ny > maxy[loc] + 80) ny = maxy[loc] + 80;

	animation++;
	if (animation >= 64) animation -= 64;
	if (dx > 0)
		animation_cycle = 0;
	else if (dx < 0)
		animation_cycle = 1;
	else if (dy > 0)
		animation_cycle = 2;
	else if (dy < 0)
		animation_cycle = 3;
	else animation = 0;

	for (int i = 0; i < 20; i++)
		if (npcs[i].image != nullptr)
		{
			if (nx + flipp.width > npcs[i].x && nx < npcs[i].x + npcs[i].width &&
				ny + flipp.height > npcs[i].y && ny < npcs[i].y + npcs[i].height)
			{
				if (nx + flipp.width < npcs[i].x + 5)
					nx = npcs[i].x - flipp.width;
				else if (nx + 5 > npcs[i].x + npcs[i].width)
					nx = npcs[i].x + npcs[i].width;
				if (ny + flipp.height < npcs[i].y + 5)
					ny = npcs[i].y - flipp.height;
				else if (ny + 5 > npcs[i].y + npcs[i].height)
					ny = npcs[i].y + npcs[i].height;
			}
		}
	if ((nx - flipp.x) * (ny - flipp.y))
	{
		dia_count++;
		if (dia_count >= 3)
		{
			dia_count -= 3;
			nx = flipp.x + (nx - flipp.x) / 2;
			ny = flipp.y + (ny - flipp.y) / 2;
		}
	}
	flipp.x = nx;
	flipp.y = ny;
	x = flipp.x < minx[loc] ? minx[loc] : flipp.x > maxx[loc] ? maxx[loc] : flipp.x;
	y = flipp.y < miny[loc] ? miny[loc] : flipp.y > maxy[loc] ? maxy[loc] : flipp.y;

	if (keys.action)
	{
		((screen_dialog*)gc.screens[1])->say(this,
			("Hey, flipp."));
		gc.set_active_screen(gc.screens[1]);
	}
}

const int start_row = 24;

void screen_map::char_draw(map_character& ch, bool flip)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += LCD_WIDTH_PX * ch.prev_y + ch.prev_x;
	for (int j = ch.prev_y; j < ch.prev_y + ch.height; j++)
	{
		for (int i = ch.prev_x; i < ch.prev_x + ch.width; i++)
		{
			if (i >= 0 && j >= 24 && i < 384 && j < 216)
				*(VRAM++) = get_pixel(x + i, y + j - start_row);
		}
		VRAM += LCD_WIDTH_PX - ch.width;
	}
	ch.prev_x = ch.x - x + 176;
	ch.prev_y = ch.y - y + start_row + 80;
	CopySprite(ch.image, ch.prev_x, ch.prev_y, ch.width, ch.height, flip, ch.mode);
}

inline color_t screen_map::get_pixel(int xc, int yc)
{
	if (loc == 1)
	{
		// castle ground
		return ((xc >> 5) & 1) + ((yc >> 5) & 1) == 1 ? COLOR_AZURE : COLOR_LIGHTBLUE;
	}
	return COLOR_WHITE;
}

void screen_map::draw()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += LCD_WIDTH_PX * start_row;

	const int sprite_no[][4] = { {0,1,0,2},{0,2,0,1},{3,4,3,4},{5,6,5,6} };
	bool flip = false;
	flipp.image = sprite_flipp[sprite_no[animation_cycle][animation / 16]];
	switch (animation_cycle)
	{
	case 0:
		flip = false;
		break;
	case 1:
		flip = true;
		break;
	case 2: case 3:
		flip = animation >= 48;
		break;
	}

	char_draw(flipp, flip);

	for (int i = 0; i < 20; i++)
		if (npcs[i].image != nullptr)
			CopySprite(npcs[i].image, npcs[i].x - x + 176, npcs[i].y - y + start_row + 80, npcs[i].width, npcs[i].height, false, npcs[i].mode);
}

void screen_map::redraw()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += LCD_WIDTH_PX * start_row;

	// draw castle
	BdispH_AreaFill(8, 16, 32, 36, COLOR_BLACK);

	for (int j = 0; j < 216 - start_row; j++)
	{
		int yc = y + j;
		for (int i = 0; i < 384; i++)
		{
			int xc = x + i;

			color_t color = COLOR_AZURE;

			if (loc == 1)
			{
				if (yc >= 8 && yc <= 58 && xc >= 8 && xc <= 91)
				{
					// castle
					if (yc <= 17 && (xc <= 36 || xc >= 63))
					{
						if (yc <= 12)
						{
							int k = (xc - 8) % 11;
							color = 
								(k == 0 || k == 6) ? COLOR_BLACK :
								k < 6 ? (yc == 8 ? COLOR_BLACK : COLOR_LIGHTGRAY) : (yc == 12 ? COLOR_BLACK : COLOR_WHITE);
						}
						else color = (xc == 8 || xc == 36 || xc == 63 || xc == 91 || yc == 17 ? COLOR_BLACK : COLOR_LIGHTGRAY);
					}
					else if (xc <= 30 || xc >= 69)
					{
						color = xc < 14 || xc > 85 ? COLOR_WHITE :
							(xc == 14 || xc == 30 || xc == 69 || xc == 85 || yc == 58 ? COLOR_BLACK : COLOR_LIGHTGRAY);
					}
					else if (xc > 30 && xc < 69 && yc >= 25 && yc <= 52)
						color = (yc == 25 || yc == 52 ? COLOR_BLACK : COLOR_LIGHTGRAY);
					else color = COLOR_WHITE;
				}
				else
				{
					// castle ground
					color = ((xc >> 5) & 1) + ((yc >> 5) & 1) == 1 ? COLOR_AZURE : COLOR_LIGHTBLUE;
					if (xc == 0 || xc == 32) color = COLOR_BLACK;
				}
			}

			*(VRAM++) = color;
		}
	}
	draw();
}