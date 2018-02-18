#pragma once
#include "character.h"

class player
{
public:
	player();
	int upgrades[100];
	int rest;

	void set_character(character& ch);
	void show_stats();

	static player pl;
};
