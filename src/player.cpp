#include "player.h"
#include "screen_battle.h"
#include "display_helper.h"

player player::pl;

player::player()
{
	memset(upgrades, 0, sizeof(upgrades));
	upgrades[3] = 1;
	upgrades[4] = 1;
	rest = 0;
}

void player::set_character(character& ch)
{
	ch.level = -1;
	for (int i = 0; i < 100; i++)
		ch.level += upgrades[i];
	ch.hp = ch.mhp = 18 + ch.level * 2;
	ch.attack = 4 + upgrades[0];
	ch.defense = 0 + upgrades[1];
	ch.speed = 2 + upgrades[2];
	ch.hpbar = HPBAR_INTERVALS;
	ch.attack_temp = ch.defense_temp = ch.speed_temp = 0;

	ch.sp = ch.msp = 1;
}

void player::set_deck()
{
	num_spells = 0;
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
	x += 6;
	PrintMiniMini(&x, &y, "HP ", 0x42, TEXT_COLOR_BLACK, 0);
	sprintf(buffer, "%d", character::bchs[0].hp);
	PrintMiniMini(&x, &y, buffer, 0x42, character::bchs[0].hp <= character::bchs[0].mhp / 3 ? TEXT_COLOR_RED : TEXT_COLOR_BLACK, 0);
	sprintf(buffer, "/ %d Atk ", character::bchs[0].mhp);
	PrintMiniMini(&x, &y, buffer, 0x42, TEXT_COLOR_BLACK, 0);
	sprintf(buffer, "%d", character::bchs[0].attack);
	PrintMiniMini(&x, &y, buffer, 0x42, character::bchs[0].attack_temp ? TEXT_COLOR_RED : TEXT_COLOR_BLACK, 0);
	PrintMiniMini(&x, &y, " Def ", 0x42, TEXT_COLOR_BLACK, 0);
	sprintf(buffer, "%d", character::bchs[0].defense);
	PrintMiniMini(&x, &y, buffer, 0x42, character::bchs[0].defense_temp ? TEXT_COLOR_RED : TEXT_COLOR_BLACK, 0);
	PrintMiniMini(&x, &y, " Spd ", 0x42, TEXT_COLOR_BLACK, 0);
	sprintf(buffer, "%d", character::bchs[0].speed);
	PrintMiniMini(&x, &y, buffer, 0x42, character::bchs[0].speed_temp ? TEXT_COLOR_RED : TEXT_COLOR_BLACK, 0);
	if (num_spells > 0)
	{
		x += 12;
		sprintf(buffer, "Card=%d/%d", cards_left, num_spells);
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