#pragma once

#include "screen.h"
#include "character.h"

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

	int effects[10][6];	// 0=type 1=x 2=y 3=count 4=prev_count
	int drawing_frame;
	int prev_drawing_frame;

	void prepare_spells();

	void draw_icons(bool draw_icons);
	void draw_spell_effect(int frame, bool draw_mode);
	int add_effect(int type, int x, int y, int color = 0);

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
	void enemy_attack(int attacker);
};