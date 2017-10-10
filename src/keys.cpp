#include <fxcg/keyboard.h>
#include <fxcg/display.h>

#include "keys.h"

#if !TARGET_WINSIM
// returns true if the key is down, false if up
bool keyDown_fast(unsigned char keyCode) {
	static const unsigned short* keyboard_register = (unsigned short*)0xA44B0000;

	int row, col, word, bit;
	row = keyCode % 10;
	col = keyCode / 10 - 1;
	word = row >> 1;
	bit = col + 8 * (row & 1);
	return (keyboard_register[word] & (1 << bit));
}
#endif

class keys_type keys;

void updateKeys()
{
	bool now, now2, now3;
	for (int i = 0; i < 6; i++)
	{
		now = keyDown_fast(79 - i * 10);
		keys.f_key[i] = (!keys.f_key_hold[i] && now);
		keys.f_key_hold[i] = now;
	}
	now = keyDown_fast(48);
	keys.menu = !keys.menu_hold && now;
	keys.menu_hold = now;
	now = keyDown_fast(10);
	keys.ac = !keys.ac_hold && now;
	keys.ac_hold = now;

	static bool prev_action[2] = { false, false };
	static bool prev_cancel[3] = { false, false, false };

	now = keyDown_fast(31);
	now2 = keyDown_fast(71);
	keys.action = (!prev_action[0] && now) || (!prev_action[1] && now2);
	keys.action_hold = now || now2;
	prev_action[0] = now;
	prev_action[1] = now2;

	now = keyDown_fast(41);
	now2 = keyDown_fast(47);
	now3 = keyDown_fast(61);
	keys.cancel = (!prev_cancel[0] && now) || (!prev_cancel[1] && now2) || (!prev_cancel[2] && now3);
	keys.cancel_hold = now || now2 || now3;
	prev_cancel[0] = now;
	prev_cancel[1] = now2;
	prev_cancel[2] = now3;

	now = keyDown_fast(38);
	keys.left = !keys.left_hold && now;
	keys.left_hold = now;
	now = keyDown_fast(27);
	keys.right = !keys.right_hold && now;
	keys.right_hold = now;
	now = keyDown_fast(28);
	keys.up = !keys.up_hold && now;
	keys.up_hold = now;
	now = keyDown_fast(37);
	keys.down = !keys.down_hold && now;
	keys.down_hold = now;
}
