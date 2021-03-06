#include "screen_battle.h"
#include "display_helper.h"
#include "keys.h"
#include "sprites.h"
#include "game_control.h"
#include "screen_dialog.h"
#include "screen_upgrade.h"
#include "player.h"
#include "utils.h"
#include <math.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

screen_battle::screen_battle()
{
	for (int i = 0; i < 10; i++)
	{
		effects[i] = effect("", 0, 0, 0, 0);
	}

	memset(character::bchs, 0, sizeof(character::bchs));

	character::bchs[0].set_image(sprite_flipp[0]);
	character::bchs[0].width = 32;
	character::bchs[0].height = 32;
}

void screen_battle::load(int troop_no)
{
	stage = troop_no;
	command_no = 0;
	target_no = 0;

	player::pl.set_character(character::bchs[0]);
	character::bchs[0].x = 70;
	character::bchs[0].y = 128;
	switch (troop_no)
	{
	case 1:
		character::bchs[1] = character(1);
		character::bchs[2] = character(1);
		break;
	case 2:
		character::bchs[1] = character(1);
		character::bchs[2] = character(1);
		character::bchs[3] = character(1);
		break;
	case 3:
		int pos = ran() % 2;
		character::bchs[1] = character(pos + 2);
		character::bchs[2] = character(4);
		character::bchs[3] = character(3 - pos);
		break;
	}
	int c = 0;
	for (c = 1; c < 10; c++)
	{
		if (character::bchs[c].mhp > 0)
		{
			character::bchs[c].x = 225 + 40 * c;
			character::bchs[c].y = 130;
			character::bchs[c].prev = c - 1;
			character::bchs[c - 1].next = c;
		}
		else break;
	}

	player::pl.cards_left = player::pl.num_spells;
}

void screen_battle::update()
{
	// update effects
	for (int i = 0; i < 10; i++)
		if (effects[i].text[0])
			effects[i].frame--;

	// update battle characters
	for (int i = 0; i < 10; i++)
		if (character::bchs[i].mhp > 0)
		{
			int k = character::bchs[i].hp * HPBAR_INTERVALS / character::bchs[i].mhp;
			if (k != character::bchs[i].hpbar)
			{
				character::bchs[i].hpbar += (character::bchs[i].hpbar < k ? 1 : -1);
				character::bchs[i].hpbar_duration = 100;
			}
			if (character::bchs[i].hpbar_duration > 1)
			{
				character::bchs[i].hpbar_duration--;
			}
			if (character::bchs[i].hp <= 0 && character::bchs[i].death_animation > 0)
			{
				character::bchs[i].death_animation++;
				if (character::bchs[i].death_animation >= 128)
					die(i);
			}
		}
	target_timer++;
}

const char messages[][12] = {
	"",
	"NICE",
	"GREAT",
	"EXCELLENT",
	"BLOCK",
	"EVADE"
};

void screen_battle::draw_spell_effect(int spell_no, int frame, int draw_mode)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	switch (spell_no)
	{
	case SPELL_FIREBALL:
		for (int k = 0; k < 7; k++)
		{
			int t = frame - 6 + k;
			if (t < 0 || t > 67) continue;
			if (t > 64) t = 64;
			int sx = (character::bchs[0].x * (64 - t) + character::bchs[target_no].x * t) / 64 - 2;
			int sy = ((character::bchs[0].y - 20) * (64 - t) + (character::bchs[target_no].y - 5) * t) / 64 + ((t - 32) * (t - 32) - 1024) / 32;

			if (draw_mode)
			{
				int radius = k * 7 + 11;
				for (int i = -7; i <= 7; i++)
					for (int j = -7; j <= 7; j++)
						if (i * i + j * j <= radius)
							VRAM[(sy + j) * LCD_WIDTH_PX + (sx + i)] = 0xF800 + (radius - i * i - j * j) * 0x20;
			}
			else BdispH_AreaFill(sx - 7, sx + 7, sy - 7, sy + 7, COLOR_WHITE);
		}
		break;
	case SPELL_ICE_LANCE:
		{
			if (frame >= 67) break;
			int t = frame > 64 ? 64 : frame;
			int sx = (character::bchs[0].x * (262144 - t * t * t) + (character::bchs[target_no].x - 5) * t * t * t) / 262144;
			int sy = ((character::bchs[0].y - 20) * (64 - t) + (character::bchs[target_no].y - 5) * t) / 64 + ((t - 32) * (t - 32) - 1024) / 64;

			if (draw_mode)
			{
				for (int i = 1; i < 31; i++)
					for (int j = 1; j < 31; j++)
					{
						if (i - j > 7 || j * 2 - i > 26) continue;
						color_t c = sprite_spells[SPELL_ICE_LANCE][(31 - j) * 32 + i];
						if (c != 0xE7DF)
							VRAM[(sy + j - 16) * LCD_WIDTH_PX + (sx + i - 16)] = c;
					}
			}
			else BdispH_AreaFill(sx - 16, sx + 15, sy - 16, sy + 15, COLOR_WHITE);
		}
		break;
	case SPELL_AERIAL:
		{
			if (frame == 72) break;
			int t = frame > 64 ? 64 : frame;
			int sx = ((character::bchs[0].x + 5) * (262144 - t * t * t) + (character::bchs[target_no].x - 5) * t * t * t) / 262144;
			int sy = ((character::bchs[0].y) * (64 - t) + (character::bchs[1].y) * t) / 64;

			if (draw_mode)
			{
				int radius = frame < 64 ? frame * 5 / 2 + 1 : (72 - frame) * 20;
				int inner_radius = radius / 3;
				int outer_radius = inner_radius * 2;

				for (int i = -12; i <= 12; i++)
					for (int j = -12; j <= 12; j++)
					{
						double c = cos(frame * 0.1);
						double s = sin(frame * 0.1);
						double x = i * c + j * s;
						double y = i * s - j * c;

						if (x * x + y * y * 6 < inner_radius)
							BdispH_SetPoint(sx + i, sy + j, 0xFFFF);
						else if (i * i + j * j <= radius)
						{
							int k1 = (int)(Q_rsqrt(1 / (1 - (double)(i * i + j * j) / radius)) * 32);
							int k2 = 32 - (i * i + j * j) * 32 / radius;
							BdispH_SetPointAlpha(sx + i, sy + j, (0x47F8 - 0x0801 * (k2 / 5)), k1);
						}
						if (x * x * 20 + y * y <= outer_radius + 32)
							BdispH_SetPointAlpha(sx + i, sy + j, 0xFFFF, 32 - abs((int)((x * x * 20 + y * y - outer_radius) * 32 / outer_radius)));
					}
			}
			else BdispH_AreaFill(sx - 12, sx + 12, sy - 12, sy + 12, COLOR_WHITE);
		}
	}
}

void screen_battle::draw()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	draw_icons(false);

	for (int i = 0; i < 10; i++)
		effects[i].draw(false);

	for (int i = 0; i < 10; i++)
	{
		character& b = character::bchs[i];
		if (b.mhp >= 0 && (b.prev_x != b.x || b.prev_y != b.y || b.death_animation >= 20 || b.need_update))
		{
			int radius = MAX(b.height - b.height / 2, b.width - b.width / 2);
			for (int y = MAX(b.prev_y - radius, 24); y < MIN(b.prev_y + radius, LCD_HEIGHT_PX); y++)
			{
				for (int x = MAX(b.prev_x - radius, 0); x < MIN(b.prev_x + radius, LCD_WIDTH_PX); x++)
				{
					VRAM[y * LCD_WIDTH_PX + x] = 0xffff;
				}
			}
			b.need_update = false;
			BdispH_AreaFill(b.prev_x - 13, b.prev_x + 13, b.prev_y + b.height / 2 + 5, b.prev_y + b.height / 2 + 10, COLOR_WHITE);
			BdispH_AreaFill(b.prev_x + 7, b.prev_x + 13, b.prev_y + b.height / 2 + 11, b.prev_y + b.height / 2 + 15, COLOR_WHITE);
		}
	}

	if (state == 4)
	{
		// spell card sliding animation deletion
		int sx = 40 * command_no + 107;
		int sy = prev_drawing_frame < 32 ? 163 + (32 - prev_drawing_frame) * (32 - prev_drawing_frame) / 32 : 163;
		BdispH_AreaFill(sx - 16, sx + 15, sy - 16, sy + 17, COLOR_WHITE);
	}
	else if (state == 5)
	{
		draw_spell_effect(command_no, prev_drawing_frame, false);
	}

	color_t c = COLOR_BLACK;
	if (state == 1)
	{
		int sth = ((target_timer + 16) % 48) - 24;
		if (sth < 0) sth = -sth;

		sth = sth * 31 / 12;
		c = ((sth / 2) << 11) + (sth << 5) + (sth / 2);
	}
	for (int i = 0; i < 10; i++)
	{
		character& b = character::bchs[i];
		if (b.mhp >= 0)
		{
			if (b.rotation)
				CopySpriteScale(b.image, b.x, b.y, b.width, b.height, b.mode, 16, 32, b.rotation);
			else if (b.death_animation < 32)
				CopySprite(b.image, b.x, b.y, b.width, b.height, b.mode);
			else
			{
				int h = 16;
				int v = 32;
				if (b.death_animation < 96)
				{
					h = (b.death_animation % 32) - 16;
					if (h < 0) h = -h;
					h = h * 2 - 16;
					v = 32;
				}
				else
				{
					h = 16;
					v = 128 - b.death_animation;
				}
				CopySpriteScale(b.image, b.x, b.y, b.width, b.height, b.mode, h, v, 0);
			}

			if (b.target_mode == 1)
				b.draw_target_border(c, state != 3);
			else if (b.target_mode == -1)
			{
				b.draw_target_border(COLOR_WHITE);
				b.target_mode = 0;
			}

			if ((b.hpbar_duration > 0 || keyDown_fast(78) || state < 2) && b.mhp > 0)
			{
				const int bar_size = 24;
				int x1 = b.x - 12;
				int x2 = x1 + bar_size;
				int y1 = b.y + b.height / 2 + 6;
				int y2 = y1 + 2;
				if (b.hpbar_duration <= 1)
				{
					if (keyDown_fast(78) || state < 2) b.hpbar_duration = 1;
					else
					{
						BdispH_AreaFill(x1 - 1, x2 + 1, y1 - 1, y2 + 1, COLOR_WHITE);
						BdispH_AreaFill(x2 - 5, x2 + 1, y2 + 3, y2 + 7, COLOR_WHITE);
						b.hpbar_duration = 0;
					}
				}
				if (b.hpbar_duration > 0)
				{
					int fill = x1 + b.hpbar * bar_size / HPBAR_INTERVALS;
					BdispH_AreaFill(x1, x2, y1 - 1, y1 - 1, COLOR_BLACK);
					BdispH_AreaFill(x1 - 1, x1 - 1, y1 - 1, y2 + 1, COLOR_BLACK);
					BdispH_AreaFill(x2 + 1, x2 + 1, y1 - 1, y2 + 1, COLOR_BLACK);
					BdispH_AreaFill(x1, x2, y2 + 1, y2 + 1, COLOR_BLACK);
					color_t c = b.hpbar >= HPBAR_INTERVALS / 2 ? COLOR_GREEN : b.hpbar > HPBAR_INTERVALS / 5 ? 0xC600 : COLOR_RED;
					BdispH_AreaFill(x1, fill, y1, y2, c);
					BdispH_AreaFill(fill + 1 - (b.hp == 0), x2, y1, y2, COLOR_WHITE);
					int n = b.hpbar * b.mhp / HPBAR_INTERVALS;
					n = MAX(n, b.hp);
					draw_small_num(1, x2 - 5, y2 + 3, n, 2);
				}
			}
			b.prev_x = b.x;
			b.prev_y = b.y;
		}
	}

	for (int i = 0; i < 10; i++)
		effects[i].draw(true);
	if (state != 2)
		player::pl.show_stats();

	if (state == 4)
	{
		// spell card sliding animation
		int sx = 40 * command_no + 107;
		int sy = drawing_frame < 32 ? 163 + (32 - drawing_frame) * (32 - drawing_frame) / 32 : 163;
		if (drawing_frame > 32)
			CopySpriteAlpha(sprite_spells[commands[command_no]], sx, sy, 32, 32, 0, 64 - drawing_frame);
		else CopySprite(sprite_spells[commands[command_no]], sx, sy, 32, 32);
		prev_drawing_frame = drawing_frame;
	}
	else if (state == 5)
	{
		draw_spell_effect(command_no, drawing_frame, true);
		prev_drawing_frame = drawing_frame;
	}
}

void screen_battle::redraw()
{
	draw_icons(true);
}

int screen_battle::routine()
{
	state = 2;
	player::pl.show_stats();
	prepare_spells();
	gc.update(true);
	{
		DmaWaitNext();
		MsgBoxPush(3);
		int x = 0;
		int y = 95;
		sprintf(buffer, "- STAGE %d -", stage);
		PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
		x = (LCD_WIDTH_PX - x) / 2;
		PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
		wait_for_key(KEY_CTRL_EXE);
		MsgBoxPop();
	}
	state = 0;

	// why do I need this to prevent crashing?
	for (int i = 0; i < 7; i++)
		gc.update();

	for (;;)
	{
		gc.update();
		switch (state)
		{
		case 0:
			if (keys.right)
			{
				do
				{
					command_no = (command_no + 1) % 7;
				} while (command_no >= 3 && !commands[command_no]);
			}
			if (keys.left)
			{
				do
				{
					command_no = (command_no + 6) % 7;
				} while (command_no >= 3 && !commands[command_no]);
			}
			if (keys.action)
			{
				if (command_no >= 3 && character::bchs[0].sp < 1)
					break;
				find_target(commands[command_no]);
				state = 1;
			}
			break;
		case 1:
			if (keys.right)
			{
				target_timer = 0;
				if (target_no != 10)
				{
					set_target_mode(target_no, -1);
					do
					{
						target_no = character::bchs[target_no].next;
						if (target_no == -1) target_no = 0;
					} while (!targetable(target_no, commands[command_no]));
				}
			}
			if (keys.left)
			{
				target_timer = 0;
				if (target_no != 10)
				{
					set_target_mode(target_no, -1);
					do
					{
						target_no--;
						if (target_no < 0) target_no = 9;
					} while (!targetable(target_no, commands[command_no]));
				}
			}
			set_target_mode(target_no, 1);

			if (keys.action)
			{
				state = 2;
				if (command_no == 0)
					attack_jump(target_no);
				else if (command_no == 1)
					attack_dive(target_no);
				else if (command_no == 2)
				{
					character::bchs[target_no].defense++;
					character::bchs[target_no].defense_temp++;
				}
				else
				{
					character::bchs[0].sp--;
					play_spell(commands[command_no], target_no);
				}
				set_target_mode(target_no, -1);

				if (command_no <= 2)
					state = 3;
				else
				{
					state = 0;
					command_no = 0;
				}
			}
			else if (keys.cancel)
			{
				state = 0;
				set_target_mode(target_no, -1);
			}
			break;
		case 3:
			// enemy phase
			// calculate enemies' moving position
			character::bchs[0].target_pos = character::bchs[0].x + 30;
			for (int i = 1; i < 10; i++)
			{
				if (character::bchs[i].mhp > 0)
				{
					int mov =
						character::bchs[i].speed == 0 ? 1 :
						character::bchs[0].speed == 0 ? 48 :
						(int)(Q_rsqrt((float)character::bchs[0].speed / character::bchs[i].speed) * 24);
					if (mov > 48) mov = 48;

					int min = character::bchs[character::bchs[i].prev].target_pos + 32;
					min = MAX(min, character::bchs[i].x - mov);
					int max = MIN(355, character::bchs[i].x + mov);
					if (character::bchs[i].next != -1)
						max = MIN(max, character::bchs[character::bchs[i].next].x - 32);
					if (max <= min) max = min + 1;
					character::bchs[i].target_pos = ran() % (max - min) + min;
					if (character::bchs[i].target_pos < character::bchs[i].x - 5)
						character::bchs[i].target_pos -= 3;
					else if (character::bchs[i].target_pos > character::bchs[i].x + 5)
						character::bchs[i].target_pos += 3;
					else character::bchs[i].target_pos = character::bchs[i].x;
				}
			}

			for (int j = 1; j < 12; j++)
				gc.update();

			// enemies move
			bool done = false;
			for (int i = 0; i < 60; i++)
			{
				done = true;
				for (int j = 1; j < 10; j++)
				{
					if (character::bchs[j].mhp > 0)
					{
						if (character::bchs[j].x != character::bchs[j].target_pos)
						{
							done = false;
							int delta = character::bchs[j].x - character::bchs[j].target_pos;
							if (i % 2 && abs(delta) < 24) continue;
							if (delta > 0)
								character::bchs[j].x--;
							else character::bchs[j].x++;
						}
					}
				}
				if (done) break;
				gc.update();
			}

			for (int j = 1; j < 10; j++)
				gc.update();

			// enemies attack
			for (int i = 1; i < 10; i++)
			{
				if (character::bchs[i].mhp > 0)
				{
					character::bchs[i].target_mode = 1;

					bool double_attack =
						!character::bchs[i].speed ? 0 :
						ran() % (2 * character::bchs[i].speed) >= character::bchs[i].speed + character::bchs[0].speed;
					bool skip = 
						!character::bchs[0].speed ? 0 :
						ran() % (2 * character::bchs[0].speed) >= character::bchs[0].speed + character::bchs[i].speed;

					if (skip)
						add_effect("Attack skipped", character::bchs[i].x, character::bchs[i].y - 17, COLOR_BLACK);
					else if (double_attack)
						add_effect("Double attack", character::bchs[i].x, character::bchs[i].y - 17, COLOR_BLACK);
					for (int j = 1; j < (skip || double_attack ? 50 : 18); j++)
						gc.update();
					character::bchs[i].target_mode = -1;
					if (!skip)
						enemy_attack(i, double_attack);
					for (int j = 1; j < 9; j++)
						gc.update();
				}
			}

			for (int j = 1; j < 12; j++)
				gc.update();

			state = 0;
			prepare_spells();

			// remove player's buffs
			character::bchs[0].attack -= character::bchs[0].attack_temp;
			character::bchs[0].defense -= character::bchs[0].defense_temp;
			character::bchs[0].speed -= character::bchs[0].speed_temp;
			character::bchs[0].attack_temp = 0;
			character::bchs[0].defense_temp = 0;
			character::bchs[0].speed_temp = 0;
			break;
		}

		bool finished = true;
		for (int i = 1; i < 10; i++)
			if (character::bchs[i].mhp > 0 && character::bchs[i].hp > 0) finished = false;
		if (finished) break;
	}

	state = 0;
	gc.update(true);
	DmaWaitNext();
	MsgBoxPush(4);
	PrintCXY(120, 42, "You Win!", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	PrintCXY(138, 66, "EXP 10", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	PrintCXY(93, 90, "Press [EXE]", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	wait_for_key(KEY_CTRL_EXE);
	MsgBoxPop();
	player::pl.set_character(character::bchs[0]);
	return 0;
}

const int sprite_walking[4] = { 0,1,0,2 };

void screen_battle::attack_jump(int target)
{
	int x1 = character::bchs[0].x;
	int y1 = character::bchs[0].y;
	int px, py;
	int nx = character::bchs[target].x;
	int ny = character::bchs[target].y - (character::bchs[target].height + character::bchs[0].height) / 2;

	for (int i = 1; i < 10; i++)
		if (character::bchs[i].mhp > 0)
		{
			px = character::bchs[i].x - 70;
			py = y1;
			break;
		}

	for (int i = 0; i < 6; i++)
		gc.update();
	for (int i = 0; character::bchs[0].x < px; i++)
	{
		character::bchs[0].x += 4;
		character::bchs[0].set_image(sprite_flipp[sprite_walking[(i / 8) % 4]]);
		gc.update();
	}
	set_target_mode(target_no, -1);
	character::bchs[0].set_image(sprite_flipp[0]);
	px = character::bchs[0].x;
	for (int i = 0; i < 6; i++)
		gc.update();

	for (int a = 0; a < 2; a++)
	{
		// 0=Initial -=failed +=success
		int action_command = 0;

		int range = 10 - a * 2;

		for (int f = 0; f < 40 + range / 2; f++)
		{
			gc.update();
			if (f <= 40)
			{
				character::bchs[0].x = (px * (40 - f) + nx * f) / 40;
				character::bchs[0].y = (py * (40 - f) + ny * f + (f - 20) * (f - 20) * 6 - 2400) / 40;
			}
			else if (action_command != 0) break;

			if ((keys.action) && action_command >= 0)
			{
				if (f < 40 - (range + 1) / 2)
					action_command = (f < 20 ? 0 : -1);
				else action_command = 1;
			}
		}
		px = nx;
		py = ny;
		damage(target, character::bchs[0].attack, action_command == 1 ? a + 1 : 0);
		if (action_command != 1)
			break;
	}

	trigger_deaths();
	for (int f = 0; f <= 40; f++)
	{
		character::bchs[0].x = (px * (40 - f) + x1 * f) / 40;
		character::bchs[0].y = (py * (40 - f) + y1 * f + (f - 20) * (f - 20) * 6 - 2400) / 40;
		gc.update();
	}

	wait_for_deaths();
}

void screen_battle::attack_dive(int target)
{
	int x1 = character::bchs[0].x;
	int y1 = character::bchs[0].y;
	int px, py;
	int nx = character::bchs[target].x - (character::bchs[target].width + character::bchs[0].height) / 2 + 2;
	int ny = character::bchs[target].y - 10;

	px = character::bchs[target].x - 150;
	py = y1;

	for (int i = 0; i < 6; i++)
			gc.update();
	for (int i = 0; character::bchs[0].x < px; i++)
	{
		character::bchs[0].x += 4;
		character::bchs[0].set_image(sprite_flipp[sprite_walking[(i / 8) % 4]]);
		gc.update();
	}
	set_target_mode(target_no, -1);
	character::bchs[0].set_image(sprite_flipp[0]);
	px = character::bchs[0].x;
	for (int i = 0; i < 4; i++)
		gc.update();
	character::bchs[0].need_update = true;
	character::bchs[0].rotation = 1;
	for (int i = 0; i < 8; i++)
		gc.update();

	// 0=Initial -=failed +=success
	int action_command = 0;

	int range = 9;

	int jump_frames = (character::bchs[target].x - px - 1) / 6;
	for (int f = 0; f < jump_frames + range / 2; f++)
	{
		gc.update();
		if (f <= jump_frames)
		{
			character::bchs[0].x = (px * (jump_frames - f) + nx * f) / jump_frames;
			character::bchs[0].y = (py * (24 - f) + ny * f + (f - 12) * (f - 12) * 2 - 288) / 24;
		}
		else if (action_command != 0) break;

		if ((keys.action) && action_command >= 0)
		{
			if (f < jump_frames + 4 - (range + 1) / 2)
				action_command = (f < jump_frames - 20 ? 0 : -1);
			else action_command = 1;
		}
	}
	px = nx;
	py = ny;
	character::bchs[0].rotation = 0;
	damage(target, action_command == 1 ? character::bchs[0].attack * 3 / 2 : character::bchs[0].attack, action_command == 1 ? 1 : 0);

	int x2 = MAX(x1, px - 50);
	bool push_done = false;
	int restd = action_command == 1 ? 40 : 20;

	for (int f = 0; ; f++)
	{
		if (f <= 16)
		{
			character::bchs[0].x = (px * (16 - f) + x2 * f) / 16;
			character::bchs[0].y = (py * (16 - f) + y1 * f + (f - 8) * (f - 8) * 4 - 256) / 16;
		}
		else if (f > 18 && f < 100)
		{
			character::bchs[0].x -= 3;
			character::bchs[0].set_image(sprite_flipp[sprite_walking[(f / 12) % 4]]);
			if (character::bchs[0].x < x1)
			{
				character::bchs[0].x = x1;
				f = 100;
				character::bchs[0].set_image(sprite_flipp[0]);
			}
		}

		if (!push_done)
			push_done = push(target, restd, character::bchs[0].attack / 2);

		if (f == 100 && push_done) break;
		gc.update();
	}

	wait_for_deaths();
}

void screen_battle::play_spell(int spell_no, int target)
{
	int prev_state = state;
	state = 4;
	prev_drawing_frame = 0;
	for (int i = 0; i <= 64; i++)
	{
		drawing_frame = i;
		gc.update(i == 64);
	}
	state = prev_state;
	commands[command_no] = 0;
	player::pl.cards_left--;
	player::pl.deck[command_no - 3] = player::pl.deck[player::pl.cards_left];

	set_target_mode(target_no, -1);

	command_no = spell_no;

	if (spell_no == SPELL_GAIN_STRENGTH)
	{
		character::bchs[target].attack++;
		character::bchs[target].attack_temp++;
	}
	else if (spell_no == SPELL_POTION)
	{
		heal(target, 10);
	}
	else if (spell_no == SPELL_FIREBALL)
	{
		state = 5;
		prev_drawing_frame = 0;
		for (int i = 0; i <= 75; i++)
		{
			drawing_frame = i;
			gc.update(i == 75);
		}
		damage(target, 6, 0);
	}
	else if (spell_no == SPELL_ICE_LANCE)
	{
		state = 5;
		prev_drawing_frame = 0;
		for (int i = 0; i <= 70; i++)
		{
			drawing_frame = i;
			gc.update(i == 70);
		}
		damage(target, 4, 0);

		if (character::bchs[target].speed > 0)
		{
			for (int i = 0; i < 24; i++)
				gc.update();
			memset(buffer, 0, sizeof(buffer));
			int sd = character::bchs[target].speed / 4;
			if (sd == 0) sd = 1;
			sprintf(buffer, "Speed-%d", sd);
			character::bchs[target].speed -= sd;
			add_effect(buffer, character::bchs[target].x - 5, character::bchs[target].y - 15, TEXT_COLOR_BLACK);
		}
	}
	else if (spell_no == SPELL_AERIAL)
	{
		state = 5;
		prev_drawing_frame = 0;
		for (int i = 0; i <= 72; i++)
		{
			drawing_frame = i;
			gc.update(i == 72);
		}
		damage(target, 3, 0);

		for (int i = 0; i < 5; i++)
			gc.update();

		int dist = 60;
		while (!push(target, dist, 2))
			gc.update();
	}
	else if (spell_no == SPELL_EARTHQUAKE)
	{
		state = 5;
		gc.update(true);
		for (int i = 0; i < 80; i++)
		{
			for (int j = 1; j < 10; j++)
				if (character::bchs[j].mhp > 0)
					character::bchs[j].x += ((i + j) % 2) * 4 - 2;
			gc.update(i == 79);
		}
		for (int j = 1; j < 10; j++)
			if (character::bchs[j].mhp > 0)
				damage(j, 3, 0);
	}

	for (int i = 0; i < 10; i++)
		gc.update();
	trigger_deaths();
	wait_for_deaths();
}

int screen_battle::damage(int target, int damage, int message)
{
	damage -= character::bchs[target].defense;
	character::bchs[target].hpbar_duration = 48;
	if (damage < 0) damage = 0;
	character::bchs[target].hp -= damage;
	if (character::bchs[target].hp < 0) character::bchs[target].hp = 0;
	if (message > 0)
		add_effect(messages[message], character::bchs[target].x - 5, character::bchs[target].y - 27, TEXT_COLOR_BLACK);
	return add_effect(damage, character::bchs[target].x - 5, character::bchs[target].y - 15, TEXT_COLOR_RED);
}

int screen_battle::heal(int target, int amount)
{
	character::bchs[target].hpbar_duration = 48;
	character::bchs[target].hp += amount;
	if (character::bchs[target].hp > character::bchs[target].mhp)
		character::bchs[target].hp = character::bchs[target].mhp;
	return add_effect(amount, character::bchs[target].x - 5, character::bchs[target].y - 15, TEXT_COLOR_BLUE);
}

void screen_battle::prepare_spells()
{
	character::bchs[0].sp = character::bchs[0].msp;
	for (int i = 0; i < 10; i++)
		commands[i] = i * (i < 3);

	if (player::pl.cards_left <= 3)
	{
		for (int i = 0; i < player::pl.cards_left; i++)
			commands[i + 3] = player::pl.deck[i];
	}
	else
	{
		for (int i = 0; i < player::pl.cards_left; i++)
		{
			int j = player::pl.cards_left - 1 - ran() % (player::pl.cards_left - i);
			int tmp = player::pl.deck[i];
			player::pl.deck[i] = player::pl.deck[j];
			player::pl.deck[j] = tmp;
		}
		for (int i = 0; i < 3 + (player::pl.num_spells >= 10); i++)
			commands[i + 3] = player::pl.deck[i];
	}
}

void screen_battle::draw_icons(int always_draw)
{
	static int prev_command_no = -1;
	static int prev_state = -1;
	if (!always_draw && prev_command_no == command_no && prev_state == state) return;
	prev_command_no = command_no;
	prev_state = state;

	BdispH_AreaFill(0, 383, 163, 215, COLOR_WHITE);

	int istart = 0;
	int iend = 6;
	if (state == 1) istart = iend = command_no;
	else if (state >= 2) iend = -1;
	for (int i = istart; i <= iend; i++)
	{
		int sx = 40 * i + 32 + (i >= 3 ? 75 : 0);
		if (i == 0 || commands[i])
			CopySprite(sprite_spells[commands[i]], sx, 195, 32, 32);

		if (command_no == i)
		{
			if (state == 0)
			{
				draw_target_border(sx, 195, 34, 34, COLOR_BLACK);
			}
			else if (state == 1)
				BdispH_AreaReverse(sx - 15, sx + 14, 180, 209);

			const char* tx = spell_name[commands[command_no]];

			int x = 0;
			int y = 163;
			PrintMiniMini(&x, &y, tx, 0x42, TEXT_COLOR_BLACK, 1);
			x = sx - x / 2;
			PrintMiniMini(&x, &y, tx, 0x42, TEXT_COLOR_BLACK, 0);
		}
	}
	if (state < 2 && player::pl.cards_left > 0)
	{
		int x = 0;
		int y = 183;
		PrintMiniMini(&x, &y, "SP", 0x52, TEXT_COLOR_BLACK, 1);
		x = 194 - x / 2;
		PrintMiniMini(&x, &y, "SP", 0x52, 0x8400, 0);
		for (int i = 0; i < character::bchs[0].msp; i++)
		{
			x = 195 + (2 * i - character::bchs[0].msp) * 5;
			y = 198;
			int size = 8;
			BdispH_AreaFill(x, x + size, y, y + size, COLOR_BLACK);
			if (i < character::bchs[0].sp)
				BdispH_AreaFill(x + 1, x + size - 1, y + 1, y + size - 1, COLOR_YELLOW);
			else
				BdispH_AreaFill(x + 1, x + size - 1, y + 1, y + size - 1, COLOR_WHITE);
		}
	}

	if (state == 3)
	{
		int x = 0;
		int y = 0;
		PrintMini(&x, &y, "- Enemy Phase -", 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
		x = 192 - x / 2;
		y = 192;
		PrintMini(&x, &y, "- Enemy Phase -", 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
	}
}

int screen_battle::add_effect(const char* text, int x, int y, int color)
{
	for (int i = 0; i < 10; i++)
	{
		if (!effects[i].text[0])
		{
			effects[i] = effect(text, x, y, 50, color);
			return i;
		}
	}
	return -1;
}

int screen_battle::add_effect(int number, int x, int y, int color)
{
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%d", number);
	return add_effect(buffer, x, y, color);
}

bool screen_battle::targetable(int target, int skill_no)
{
	if (skill_no >= 2 && skill_no <= 4) return !target;
	if (target < 1 || target >= 10) return false;
	if (character::bchs[target].mhp <= 0) return false;
	if (skill_no == 1 || skill_no == 7)
		return target == character::bchs[0].next;
	return true;
}

void screen_battle::find_target(int skill_no)
{
	target_timer = 0;
	if (skill_no == 8)
	{
		target_no = 10;
		return;
	}
	if (target_no < 0 || target_no > 9) target_no = 0;
	while (!targetable(target_no, skill_no))
	{
		target_no = character::bchs[target_no].next;
		if (target_no > 9) target_no = 0;
	}
}

void screen_battle::trigger_deaths()
{
	for (int i = 1; i < 10; i++)
		if (character::bchs[i].mhp > 0 && character::bchs[i].hp <= 0)
		{
			if (character::bchs[i].death_animation < 1)
				character::bchs[i].death_animation = 1;
		}
}

void screen_battle::wait_for_deaths()
{
	bool finished;
	do
	{
		gc.update();
		finished = true;
		for (int i = 1; i < 10; i++)
			if (character::bchs[i].mhp > 0 && character::bchs[i].hp <= 0)
			{
				finished = false;
			}
	} while (!finished);
}

void screen_battle::die(int target)
{
	character::bchs[target].mhp = 0;
	if (character::bchs[target].prev != -1)
		character::bchs[character::bchs[target].prev].next = character::bchs[target].next;
	if (character::bchs[target].next != -1)
		character::bchs[character::bchs[target].next].prev = character::bchs[target].prev;
}

bool screen_battle::push(int& target, int& dist, int d)
{
	int target2 = character::bchs[target].next;
	int nowd = MIN(1 + dist / 10, dist);
	dist -= nowd;
	character::bchs[target].x += nowd;
	if (character::bchs[target].x > 360) character::bchs[target].x = 360;

	if (target2 != -1)
	{
		int diff = character::bchs[target].x + (character::bchs[target].width + character::bchs[target2].width) / 2 - character::bchs[target2].x + 1;
		if (diff >= 0)
		{
			for (int i = 0; i < 3; i++)
				gc.update();
			dist = (dist + diff) * 2 / 3;
			if (dist < 10) dist = 10;

			d = MIN(d, dist * d / 10);
			damage(target, d, 0);
			trigger_deaths();
			damage(target2, d, 0);

			target = target2;
			for (target2 = target + 1; target2 < 10; target2++)
				if (character::bchs[target2].mhp > 0) break;
		}
	}
	if (dist == 0)
	{
		trigger_deaths();
		return true;
	}
	return false;
}

void screen_battle::set_target_mode(int target_no, int target_mode)
{
	if (target_no == 10)
	{
		for (int i = 1; i < 10; i++)
			if (character::bchs[i].mhp > 0)
				character::bchs[i].target_mode = target_mode;
	}
	else character::bchs[target_no].target_mode = target_mode;
}

void screen_battle::enemy_attack(int attacker, int double_attack)
{
	int x1 = character::bchs[attacker].x;
	int y1 = character::bchs[attacker].y;
	int nx = character::bchs[0].x;
	int ny = character::bchs[0].y - (character::bchs[attacker].height + character::bchs[0].height) / 2;
	int px = x1;
	int py = y1;

	for (int i = 0; i < 6; i++)
		gc.update();

	for (int a = 0; a < 1 + double_attack; a++)
	{
		// 0=Initial -=failed +=success
		int action_command = 0;

		int range = 8 + a * 2;

		for (int f = 0; f < 40 + range / 2; f++)
		{
			if (f <= 40)
			{
				character::bchs[attacker].x = (px * (40 - f) + nx * f) / 40;
				character::bchs[attacker].y = (py * (40 - f) + ny * f + (f - 20) * (f - 20) * 6 - 2400) / 40;
				if (a == 0 && f == 13)
					character::bchs[attacker].rotation = 3;
				else if (a == 0 && f == 26)
					character::bchs[attacker].rotation = 2;
			}
			else if (action_command != 0) break;

			if ((keys.action) && action_command == 0)
			{
				if (f < 40 - (range + 1) / 2)
					action_command = (f < 24 ? 0 : -1);
				else
				{
					int r = ran() % (character::bchs[attacker].attack + 1);
					if (f == 40 && r <= character::bchs[0].defense * 2 ||
						(f == 39 || f == 41) && r < character::bchs[0].defense * 2 - character::bchs[attacker].attack)
						action_command = 2;
					else action_command = 1;
				}
			}
			if ((keys.cancel) && action_command == 0)
			{
				if (f != 40)
					action_command = (f < 28 ? 0 : -1);
				else action_command = 2;
			}
			gc.update();
		}
		px = nx;
		py = ny;
		if (action_command == 2)
		{
			add_effect(messages[5], character::bchs[0].x - 5, character::bchs[0].y - 20, TEXT_COLOR_BLACK);
		}
		else if (action_command == 3)
		{
			// unused "Counter"
			character::bchs[attacker].rotation = 0;
			int ep = damage(attacker, (character::bchs[0].attack + character::bchs[0].defense) / 2, 0);
			for (int i = 0; i < 20; i++)
			{
				if (i < 5)
				{
					character::bchs[attacker].x += 8 - i;
					character::bchs[attacker].y--;
					effects[ep].x += 8 - i;
				}
				gc.update();
			}
			break;
		}
		else damage(0, character::bchs[attacker].attack - (action_command > 0), action_command > 0 ? 4 : 0);
	}

	trigger_deaths();
	wait_for_deaths();

	if (character::bchs[attacker].mhp > 0)
	{
		px = character::bchs[attacker].x;
		py = character::bchs[attacker].y;
		for (int f = 0; f <= 40; f++)
		{
			if (f == 13 && character::bchs[attacker].rotation)
				character::bchs[attacker].rotation = 3;
			else if (f == 27)
				character::bchs[attacker].rotation = 0;
			character::bchs[attacker].x = (px * (40 - f) + x1 * f) / 40;
			character::bchs[attacker].y = (py * (40 - f) + y1 * f + (f - 20) * (f - 20) * 7 - 2800) / 40;
			gc.update();
		}
	}
}
