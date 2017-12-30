#pragma once

class screen;

class game_control
{
public:
	screen* screens[10];
	screen* active_screen;
	int clock;
	int prev_time;

	void start();
	void redraw();
	void pause();
	void update();
	void draw();
	void set_active_screen(screen* screen);

private:
	int fps;
	int fps_count[2];
	int last_draw;
};

extern game_control gc;