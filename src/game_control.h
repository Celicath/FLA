#pragma once

#include "screen_battle.h"
#include "screen_map.h"
#include "screen_dialog.h"

class screen;

class game_control
{
public:
	screen* screens[10];
	screen* active_screen;
	int clock;

	void start();
	void redraw();
	void pause();
	void update();
	void draw();
	void set_active_screen(screen* screen);

private:
	int prev_time;
	int fps;
	int fps_count[2];
	int last_draw;
};

extern game_control gc;