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

	player::pl.show_stats();
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