#pragma once

bool keyDown_fast(unsigned char keyCode);

// refresh key values
void updateKeys();

struct keys_type {
	bool f_key[7];	// F1~F6. 0=no press, 1~6=Fn pressed
	bool action;	// Action key. EXE or 0
	bool cancel;	// Cancel key. EXIT or . or (-)
	bool menu;		// Menu key. MENU
	bool ac;		// AC key. AC

	bool left;
	bool right;
	bool up;
	bool down;

	bool f_key_hold[7];
	bool action_hold;
	bool cancel_hold;
	bool menu_hold;
	bool ac_hold;

	bool left_hold;
	bool right_hold;
	bool up_hold;
	bool down_hold;
};

extern keys_type keys;
