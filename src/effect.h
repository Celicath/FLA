#pragma once

class effect {
public:
	effect() { }
	effect(const char* text, int x, int y, int frame, int color);
	void draw(int draw_mode);

	char text[20];
	int x;
	int y;
	int frame;
	int prev_frame;
	int color;
};
