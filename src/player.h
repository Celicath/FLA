#pragma once
#include "character.h"

class player
{
public:
	player();
	/*
	0~2 = Level
	3 = 
	*/
	int upgrades[100];
	int rest;

	void set_character(character& ch);

	static player pl;
};
