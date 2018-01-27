#include "screen_briefing.h"
#include "game_control.h"
#include "player.h"
#include "character.h"

screen_briefing::screen_briefing()
{
}

const char enemies[][20]
{
	"Shloom",
	"Red Shloom",
	"Blue Shloom",
	"Big Shloom",
};

void screen_briefing::load(int level_)
{
	level = level_;
	switch (level)
	{
	case 1:
		break;
	}
}

void screen_briefing::update()
{
}

void screen_briefing::draw()
{
	redraw();
}

void screen_briefing::redraw()
{
	char buffer[20];
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Wave %d", level);
	int x = 15;
	int y = 10;
	PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	x = 15;
	y = 40;
	PrintMiniMini(&x, &y, "Enemy list:", 0x50, TEXT_COLOR_BLACK, 0);
	for (int i = 0; i < 10; i++)
	{
		x = 20;
		y = 60 + i * 12;
		PrintMiniMini(&x, &y, "- Shloom", 0x50, TEXT_COLOR_BLACK, 0);
	}

	const int charstatx = 260;
	x = charstatx;
	y = 10;
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Flipp Lv.%d", character::bchs[0].level);
	PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);

	for (int i = 0; i < 4; i++)
	{
		x = charstatx + 5;
		y = 12 * i + 40;
		PrintMiniMini(&x, &y,
			i == 0 ? "HP" : i == 1 ? "Atk" : i == 2 ? "Def" : "Spd",
			0x50, TEXT_COLOR_BLACK, 0);
		x = 0;
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "%d",
			i == 0 ? character::bchs[0].mhp :
			i == 1 ? character::bchs[0].attack :
			i == 2 ? character::bchs[0].defense : character::bchs[0].speed
		);
		PrintMiniMini(&x, &y, buffer, 0x50, TEXT_COLOR_BLACK, 1);
		x = 367 - x;
		PrintMiniMini(&x, &y, buffer, 0x50, TEXT_COLOR_BLACK, 0);
	}
}

int screen_briefing::routine()
{
	gc.update(true);
	player::pl.set_character(character::bchs[0]);

	DmaWaitNext();
	int key;
	do
	{
		GetKey(&key);
	} while (key != KEY_CTRL_EXE);

	return 2;
}