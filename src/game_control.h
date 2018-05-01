#pragma once

class screen;

class game_control
{
public:
	screen* screens[8];
	screen* active_screen;
	int clock;
	int prev_time;

	void start();
	void redraw();
	void pause();
	void update(int always_draw = 0);
	void draw();
	void set_active_screen(screen* screen);

private:
	int fps;
	int fps_count[2];
	int last_draw;
};

extern game_control gc;