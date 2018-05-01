#pragma once

#include "screen.h"
#include "character.h"
#include "effect.h"

class screen_battle : public screen {
public:
	screen_battle();
	char text[256];
	void load(int troop_no);
	virtual void update() override;
	virtual void draw() override;
	virtual void redraw() override;
	virtual int routine() override;
private:
	int stage;
	int command_no;
	int target_no;
	int state;
	int target_timer;

	int spells[4]; // currently drawn spells

	effect effects[10];

	char buffer[20];

	int drawing_frame;
	int prev_drawing_frame;

	void prepare_spells();

	void draw_icons(int always_draw);
	void draw_spell_effect(int spell_no, int frame, int draw_mode);
	int add_effect(const char* text, int x, int y, int color);
	int add_effect(int number, int x, int y, int color);

	bool targetable(int target, int skill_no);
	void find_target(int skill_no);

	// helper functions
	int damage(int target, int damage, int message);
	int heal(int target, int damage);
	void trigger_deaths();
	void wait_for_deaths();
	void die(int target);

	// attacks
	void attack_jump(int target);
	void attack_dive(int target);
	void play_spell(int spell_no, int target);

	// enemy attack
	void enemy_attack(int attacker, int double_attack);
};