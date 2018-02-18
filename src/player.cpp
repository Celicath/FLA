#include "player.h"
#include "screen_battle.h"
#include "display_helper.h"

player player::pl;

player::player()
{
	memset(upgrades, 0, sizeof(upgrades));
	rest = 0;
}

void player::set_character(character& ch)
{
	ch.level = (upgrades[0] + upgrades[1] + upgrades[2]) + 1;
	ch.hp = ch.mhp = 10 + ch.level * 2;
	ch.attack = 4 + upgrades[0];
	ch.defense = 0 + upgrades[1];
	ch.speed = 2 + upgrades[2];
}

void player::show_stats()
{
	const int charstatx = 260;

	BdispH_AreaFill(charstatx - 5, charstatx - 5, 0, LCD_HEIGHT_PX - 1, COLOR_BLACK);

	char buffer[20];

	int x = charstatx;
	int y = 10;
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