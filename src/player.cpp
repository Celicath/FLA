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
	ch.hpbar = HPBAR_INTERVALS;
}

void player::set_deck()
{
	num_spells = 2;
	deck[0] = 1;
	deck[1] = 2;
	for (int i = 3; i < 100; i++)
	{
		for (int j = 0; j < upgrades[i]; j++)
		{
			deck[num_spells++] = i;
		}
	}
}

void player::show_stats()
{
	char buffer[60];

	for (int i = 0; i < 16; i++)
		BdispH_AreaFill(0, 383, i, i, 0x0841 * (31 - i * i / 13));

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Flip Lv.%d", character::bchs[0].level);
	int x = 2;
	int y = 2;
	PrintMiniMini(&x, &y, buffer, 0x52, TEXT_COLOR_BLACK, 0);
	sprintf(buffer, "HP %d/%d Atk %d Def %d Spd %d",
		character::bchs[0].hp,
		character::bchs[0].mhp,
		character::bchs[0].attack,
		character::bchs[0].defense,
		character::bchs[0].speed);
	x += 6;
	PrintMiniMini(&x, &y, buffer, 0x42, TEXT_COLOR_BLACK, 0);
	if (num_spells > 0)
	{
		x += 12;
		sprintf(buffer, "Card=%d", num_spells);
		PrintMiniMini(&x, &y, buffer, 0x42, TEXT_COLOR_BLACK, 0);
	}
	/*
	x = 0;
	y += 12;
	sprintf(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()");
	PrintMiniMini(&x, &y, buffer, 0x42, TEXT_COLOR_BLACK, 0);
	x = 0;
	y += 12;
	sprintf(buffer, "abcdefghijklmnopqrstuvwxyz1234567890");
	PrintMiniMini(&x, &y, buffer, 0x42, TEXT_COLOR_BLACK, 0);
	*/
}