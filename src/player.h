#pragma once
#include "character.h"

class player
{
public:
	player();
	int spells[80];
	int passives[80];
	int num_spells;
	int cards_left;
	int deck[80];

	int rest;

	void init_player();
	void set_character(character& ch);
	void show_stats();
	void set_deck();

	static player pl;
};
