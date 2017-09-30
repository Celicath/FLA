#pragma once

class screen;

class game_control
{
public:
	void start();
	void redraw();
	void pause();
	void update();
	void draw();

	void set_active_screen(screen* screen);

	screen* screens[10];
	screen* active_screen;

	int clock;
private:
	int prev_time;
	int fps;
	int fps_count[2];
	int last_draw;
};

extern game_control gc;