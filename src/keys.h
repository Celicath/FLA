#pragma once

bool keyDown_fast(unsigned char keyCode);

// refresh key values
void updateKeys();

struct keys_type {
	bool f_key[6];	// F1~F6 mapped to 0~5
	bool action;	// Action key. EXE or 0
	bool cancel;	// Cancel key. EXIT or . or (-)
	bool menu;		// Menu key. MENU
	bool ac;		// AC key. AC

	bool left;
	bool right;
	bool up;
	bool down;

	bool f_key_hold[6];
	bool action_hold;
	bool cancel_hold;
	bool menu_hold;
	bool ac_hold;

	bool left_hold;
	bool right_hold;
	bool up_hold;
	bool down_hold;
	bool padding[2];
};

extern keys_type keys;
