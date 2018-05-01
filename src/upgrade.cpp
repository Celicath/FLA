#include "upgrade.h"
#include "platform.h"
#include "display_helper.h"
#include "sprites.h"

const char spell_name[][16] =
{
	"Jump",
	"Dive",
	"Defend",
	"Gain Strength",
	"Potion",
	"Fireball",
	"Ice Lance",
	"Aerial",
	"Earthquake"
};
const char passive_name[][16] =
{
	"Attack Plus",
	"Defense Plus",
	"Speed Plus",
};

/* 0=passive 1=normal 2=fire 3=ice 4=wind 5=earth */
const int spell_type[] =
{
	0, 0, 0, 0, 0, 1, 2, 3, 4
};
const char type_name[][16] =
{
	"Spell",
	"Spell(Fire)",
	"Spell(Ice)",
	"Spell(Wind)",
	"Spell(Earth)",
};
const char passive_type[] = "Passive";
const color_t type_colors[] = { COLOR_WHITE, 0xFE18, 0xE71F, 0xC7F8, 0xFFF0 };
//const color_t type_colors2[] = { COLOR_WHITE, 0xF800, 0x001F, 0x0600, 0xC600 };


void upgrade::get_description(char* buffer)
{
	memset(buffer, 0, sizeof(buffer));

	if (type == SPELL)
	{
		switch (no)
		{
		case SPELL_GAIN_STRENGTH: sprintf(buffer, "Increase your attack by 1 this turn."); break;
		case SPELL_POTION: sprintf(buffer, "Restore 10 HP."); break;
		case SPELL_FIREBALL: sprintf(buffer, "6 damage to an enemy."); break;
		case SPELL_ICE_LANCE: sprintf(buffer, "4 damage + slow to an enemy."); break;
		case SPELL_AERIAL: sprintf(buffer, "3 damage + knockback to front enemy."); break;
		case SPELL_EARTHQUAKE: sprintf(buffer, "3 damage to all enemies."); break;
		}
	}
	else if (type == PASSIVE)
	{
		switch (no)
		{
		case PASSIVE_ATTACK: case PASSIVE_DEFENSE: case PASSIVE_SPEED:
			sprintf(buffer, "Increase %s by 1.", no == PASSIVE_ATTACK ? "Attack" : no == PASSIVE_DEFENSE ? "Defense" : "Speed");
			break;
		}
	}
}

const char* upgrade::get_name()
{
	return type == SPELL ? spell_name[no] : passive_name[no];
}

const char* upgrade::get_type()
{
	return type == SPELL ? type_name[spell_type[no]] : passive_type;
}

void upgrade::draw(int x, int y)
{
	CopySprite(type == SPELL ? sprite_spells[no] : sprite_passives[no], x, y, 32, 32);
	if (selected)
		draw_target_border(x, y, 32, 32, COLOR_BLACK);
}
