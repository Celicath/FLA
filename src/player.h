#pragma once
#include "character.h"

class player
{
public:
	player();
	int upgrades[100];
	int num_spells = 0;
	int cards_left = 0;
	int deck[80];

	int rest;

	void set_character(character& ch);
	void show_stats();
	void set_deck();

	static player pl;
};
