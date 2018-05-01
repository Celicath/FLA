#pragma once

class upgrade
{
public:
enum type_enum { SPELL, PASSIVE };
	type_enum type;
	int no;
	int selected;

	upgrade() { }
	upgrade(type_enum type, int no) : type(type), no(no), selected(false) { }

	void draw(int x, int y);

	void get_description(char* buffer);

	const char* get_name();
	const char* get_type();
};

enum spell_enum
{
	SPELL_JUMP,
	SPELL_DIVE,
	SPELL_DEFEND,
	SPELL_GAIN_STRENGTH,
	SPELL_POTION,
	SPELL_FIREBALL,
	SPELL_ICE_LANCE,
	SPELL_AERIAL,
	SPELL_EARTHQUAKE,
};

enum passive_enum
{
	PASSIVE_ATTACK,
	PASSIVE_DEFENSE,
	PASSIVE_SPEED,
};

extern const char spell_name[][16];
extern const char passive_name[][16];
