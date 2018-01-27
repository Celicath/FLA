#include "player.h"
#include "screen_battle.h"

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

player* player::pl = new player();