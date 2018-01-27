#include "screen_battle.h"
#include "display_helper.h"
#include "keys.h"
#include "sprites.h"
#include "game_control.h"
#include "screen_dialog.h"
#include "player.h"
#include "utils.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

screen_battle::screen_battle()
{
	for (int i = 0; i < 10; i++)
		effects[i][0] = 0;
}

void screen_battle::load(int troop_no)
{
	command_no = 0;
	target_no = 0;

	memset(character::bchs, 0, sizeof(character::bchs));

	character::bchs[0].set_image(sprite_flipp[0]);
	player::pl->set_character(character::bchs[0]);
	character::bchs[0].width = 32;
	character::bchs[0].height = 32;
	character::bchs[0].hp = character::bchs[0].mhp;
	character::bchs[0].x = 70;
	character::bchs[0].y = 138;
	switch (troop_no)
	{
	case 1:
		character::bchs[1] = character(0);
		character::bchs[2] = character(0);
		break;
	case 2:
		character::bchs[1] = character(0);
		character::bchs[2] = character(0);
		character::bchs[3] = character(0);
		break;
	case 3:
		int pos = ran() % 2;
		character::bchs[1] = character(pos + 1);
		character::bchs[2] = character(3);
		character::bchs[3] = character(2 - pos);
		break;
	}
	int c = 0;
	for (c = 1; c < 10; c++)
	{
		if (character::bchs[c].mhp > 0)
		{
			character::bchs[c].x = 225 + 40 * c;
			character::bchs[c].y = 140;
			character::bchs[c].prev = c - 1;
			character::bchs[c - 1].next = c;
		}
		else break;
	}
	for (int i = 1; i < c; i++)
		character::bchs[i].wait_gauge = 1024 * (c - i - 1) / (c - 1);
}

void screen_battle::update()
{
	// update effects
	for (int i = 0; i < 10; i++)
		if (effects[i][0])
			effects[i][3]--;

	// update battle characters
	for (int i = 0; i < 10; i++)
		if (character::bchs[i].mhp > 0)
		{
			int k = character::bchs[i].hp * HPBAR_SIZE / character::bchs[i].mhp;
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
	"NICE",
	"GREAT",
	"EXCELLENT",
	"EARLY",
	"LATE",
	"COUNTER"
};

void screen_battle::draw()
{
	color_t* VRAM = (color_t*)GetVRAMAddress();

	draw_icons(false);

	for (int i = 0; i < 10; i++)
		if (effects[i][0])
		{
			int x = effects[i][1];
			int y = effects[i][2] - 15 + (effects[i][4] * effects[i][4]) / 60;
			BdispH_AreaFill(x - 72, x + 64, y, y + 18, COLOR_WHITE);
		}

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
			BdispH_AreaFill(b.prev_x - 13, b.prev_x + 13, b.prev_y + b.height / 2 + 2, b.prev_y + b.height / 2 + 5, COLOR_WHITE);
		}
	}
	color_t c = COLOR_BLACK;
	if (state == 1)
	{
		int sth = ((target_timer + 16) % 48) - 24;
		if (sth < 0) sth = -sth;

		sth = sth * 63 / 24;
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
				b.draw_target_border(c);
			else if (b.target_mode == -1)
			{
				b.draw_target_border(COLOR_WHITE);
				b.target_mode = 0;
			}

			if ((b.hpbar_duration > 0 || keyDown_fast(78)) && b.mhp > 0)
			{
				int x1 = b.x - 12;
				int x2 = x1 + 24;
				int y1 = b.y + b.height / 2 + 3;
				int y2 = y1 + 1;
				if (b.hpbar_duration <= 1)
				{
					if (keyDown_fast(78)) b.hpbar_duration = 1;
					else
					{
						BdispH_AreaFill(x1 - 1, x2 + 1, y1 - 1, y2 + 1, COLOR_WHITE);
						b.hpbar_duration = 0;
					}
				}
				if (b.hpbar_duration > 0)
				{
					int fill = x1 + b.hpbar * 24 / HPBAR_SIZE;
					BdispH_AreaFill(x1, x2, y1 - 1, y1 - 1, COLOR_BLACK);
					BdispH_AreaFill(x1 - 1, x1 - 1, y1 - 1, y2 + 1, COLOR_BLACK);
					BdispH_AreaFill(x2 + 1, x2 + 1, y1 - 1, y2 + 1, COLOR_BLACK);
					BdispH_AreaFill(x1, x2, y2 + 1, y2 + 1, COLOR_BLACK);
					color_t c = b.hpbar >= HPBAR_SIZE / 2 ? COLOR_GREEN : b.hpbar > HPBAR_SIZE / 5 ? 0xC600 : COLOR_RED;
					BdispH_AreaFill(x1, fill, y1, y2, c);
					BdispH_AreaFill(fill + 1 - (b.hp == 0), x2, y1, y2, COLOR_WHITE);
				}
			}
			b.prev_x = b.x;
			b.prev_y = b.y;
		}
	}

	for (int i = 0; i < 10; i++)
		if (effects[i][0])
		{
			if (effects[i][3] < 0)
				effects[i][0] = 0;
			else
			{
				char buffer[20];
				memset(buffer, 0, sizeof(buffer));
				if (effects[i][0] > 0)
					sprintf(buffer, "%d", effects[i][0]);
				else sprintf(buffer, messages[-effects[i][0] - 1], effects[i][0]);
				int x = effects[i][1] - strlen(buffer) * 8;
				int y = effects[i][2] - 15 + (effects[i][3] * effects[i][3]) / 60;
				PrintMini(&x, &y, buffer, 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
				effects[i][4] = effects[i][3];
			}
		}
}

const char command_text[][8] =
{
	"Jump",
	"Dive",
	"",
	"",
	"Cards",
	"Status"
};

void screen_battle::redraw()
{
	draw_icons(true);
}

int screen_battle::routine()
{
	state = 0;
	for (;;)
	{
		gc.update();
		switch (state)
		{
		case 0:
			if (keys.right && command_no < 5)
			{
				command_no++;
			}
			if (keys.left && command_no > 0)
			{
				command_no--;
			}
			
			if (keys.action)
			{
				find_target(command_no);
				state = 1;
			}
			else
			{
				for (int i = 0; i < 6; i++)
					if (keys.f_key[i])
					{
						command_no = i;
						find_target(command_no);
						state = 1;
					}
			}
			break;
		case 1:
			if (keys.right)
			{
				target_timer = 0;
				character::bchs[target_no].target_mode = -1;
				do
				{
					target_no = character::bchs[target_no].next;
					if (target_no == -1) target_no = character::bchs[0].next;
				} while (!targetable(target_no, command_no));
			}
			if (keys.left)
			{
				target_timer = 0;
				character::bchs[target_no].target_mode = -1;
				do
				{
					target_no--;
					if (target_no < 1) target_no = 9;
				} while (!targetable(target_no, command_no));
			}
			character::bchs[target_no].target_mode = 1;

			if (keys.action || keys.f_key[command_no])
			{
				state = 2;
				if (command_no == 0)
					attack_jump(target_no);
				else if (command_no == 1)
					attack_dive(target_no);
				else character::bchs[target_no].target_mode = -1;

				state = 3;
			}
			else if (keys.cancel)
			{
				state = 0;
				character::bchs[target_no].target_mode = -1;
			}
			else
			{
				for (int i = 0; i < 6; i++)
					if (keys.f_key[i])
					{
						character::bchs[target_no].target_mode = -1;
						command_no = i;
						find_target(command_no);
					}
			}
			break;
		case 3:
			// enemy phase
			for (int j = 1; j < 12; j++)
				gc.update();

			for (int i = 0; i < 30; i++)
			{
				for (int j = 1; j < 10; j++)
				{
					if (character::bchs[j].mhp > 0 && character::bchs[j].x > character::bchs[character::bchs[j].prev].x + 30)
						character::bchs[j].x--;
				}
				gc.update();
				gc.update();
			}

			// enemy attacks
			for (int j = 1; j < 10; j++)
			{
				if (character::bchs[j].mhp > 0)
				{
					character::bchs[j].wait_gauge += (int)Q_rsqrt((float)character::bchs[0].speed / (character::bchs[j].speed << 20));
					while (character::bchs[j].wait_gauge >= 1024)
					{
						character::bchs[j].target_mode = 1;
						for (int j = 1; j < 17; j++)
							gc.update();
						character::bchs[j].target_mode = -1;
						character::bchs[j].wait_gauge -= 1024;
						enemy_attack(j);
						for (int j = 1; j < 8; j++)
							gc.update();
					}
				}
			}

			for (int j = 1; j < 12; j++)
				gc.update();

			state = 0;
			break;
		}

		bool finished = true;
		for (int i = 1; i < 10; i++)
			if (character::bchs[i].mhp > 0 && character::bchs[i].hp > 0) finished = false;
		if (finished) break;
	}

	state = 0;
	DmaWaitNext();
	MsgBoxPush(4);
	PrintCXY(120, 42, "You Win!", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	PrintCXY(138, 66, "EXP 10", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	PrintCXY(84, 90, "Press [EXIT]", TEXT_MODE_NORMAL, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
	int key;
	do
	{
		GetKey(&key);
	} while (key != KEY_CTRL_EXIT);
	MsgBoxPop();
	do
	{
		GetKey(&key);
	} while (key != KEY_CTRL_EXIT);
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
	character::bchs[target_no].target_mode = -1;
	character::bchs[0].set_image(sprite_flipp[0]);
	px = character::bchs[0].x;
	for (int i = 0; i < 6; i++)
		gc.update();

	for (int a = 0; a < 3; a++)
	{
		// 0=Initial -=failed +=success
		int action_command = 0;

		int range = 10 - a * 2;

		for (int f = 0; f < 40 + range / 2; f++)
		{
			if (f <= 40)
			{
				character::bchs[0].x = (px * (40 - f) + nx * f) / 40;
				character::bchs[0].y = (py * (40 - f) + ny * f + (f - 20) * (f - 20) * 7 - 2800) / 40;
			}
			else if (action_command != 0) break;

			if ((keys.action || keys.f_key[command_no]) && action_command >= 0)
			{
				if (f < 40 - (range + 1) / 2)
					action_command = (f < 20 ? 0 : -1);
				else action_command = 1;
			}
			gc.update();
		}
		px = nx;
		py = ny;
		damage(target, character::bchs[0].attack, action_command == 1 ? -a - 1 : 0);
		if (action_command != 1)
			break;
	}

	trigger_deaths();
	for (int f = 0; f <= 40; f++)
	{
		character::bchs[0].x = (px * (40 - f) + x1 * f) / 40;
		character::bchs[0].y = (py * (40 - f) + y1 * f + (f - 20) * (f - 20) * 7 - 2800) / 40;
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
	character::bchs[target_no].target_mode = -1;
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
		if (f <= jump_frames)
		{
			character::bchs[0].x = (px * (jump_frames - f) + nx * f) / jump_frames;
			character::bchs[0].y = (py * (24 - f) + ny * f + (f - 12) * (f - 12) * 2 - 288) / 24;
		}
		else if (action_command != 0) break;

		if ((keys.action || keys.f_key[command_no]) && action_command >= 0)
		{
			if (f < jump_frames + 4 - (range + 1) / 2)
				action_command = (f < jump_frames - 20 ? 0 : -1);
			else action_command = 1;
		}
		gc.update();
	}
	px = nx;
	py = ny;
	character::bchs[0].rotation = 0;
	damage(target, action_command == 1 ? character::bchs[0].attack * 3 / 2 : character::bchs[0].attack, action_command == 1 ? -1 : 0);

	int restd = action_command == 1 ? 40 : 20;

	int x2 = MAX(x1, px - 50);
	bool push_done = false;

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

		int target2 = character::bchs[target].next;
		if (!push_done)
		{
			int nowd = MIN(1 + restd / 10, restd);
			restd -= nowd;
			character::bchs[target].x += nowd;
			if (character::bchs[target].x > 360) character::bchs[target].x = 360;

			if (target2 != -1)
			{
				int diff = character::bchs[target].x + (character::bchs[target].width + character::bchs[target2].width) / 2 - character::bchs[target2].x + 1;
				if (diff >= 0)
				{
					restd = (restd + diff) * 2 / 3;
					if (restd < 10) restd = 10;

					damage(target, character::bchs[0].attack / 2, 0);
					trigger_deaths();
					damage(target2, character::bchs[0].attack / 2, 0);

					target = target2;
					for (target2 = target + 1; target2 < 10; target2++)
						if (character::bchs[target2].mhp > 0) break;
				}
			}
			if (restd == 0)
			{
				trigger_deaths();
				push_done = true;
			}
		}
		if (f == 100 && push_done) break;
		gc.update();
	}

	wait_for_deaths();
}

int screen_battle::damage(int target, int damage, int message)
{
	damage -= character::bchs[target].defense;
	character::bchs[target].hpbar_duration = 48;
	if (damage < 0) damage = 0;
	character::bchs[target].hp -= damage;
	if (character::bchs[target].hp < 0) character::bchs[target].hp = 0;
	if (message < 0)
		add_effect(message, character::bchs[target].x - 5, character::bchs[target].y - 32);
	return add_effect(damage, character::bchs[target].x - 5, character::bchs[target].y - 15);
}

void screen_battle::draw_icons(bool always_draw)
{
	static int prev_command_no = -1;
	static int prev_state = -1;
	if (!always_draw && prev_command_no == command_no && prev_state == state) return;
	prev_command_no = command_no;
	prev_state = state;

	BdispH_AreaFill(0, 383, 155, 215, COLOR_WHITE);

	int istart = 0;
	int iend = 5;
	if (state == 1) istart = iend = command_no;
	else if (state >= 2) iend = -1;
	for (int i = istart; i <= iend; i++)
	{
		BdispH_AreaFill(64 * i + 2, 64 * i + 60, 173, 173, COLOR_BLACK);
		BdispH_AreaFill(64 * i + 1, 64 * i + 1, 173, 214, COLOR_BLACK);

		BdispH_AreaFill(64 * i + 3, 64 * i + 3, 175, 179, COLOR_BLACK);
		BdispH_AreaFill(64 * i + 4, 64 * i + 5, 175, 175, COLOR_BLACK);
		BdispH_AreaFill(64 * i + 4, 64 * i + 5, 177, 177, COLOR_BLACK);
		Draw_SmallNum(1, 64 * i + 7 - !i, 175, i + 1, 1);

		CopySprite(sprite_command[i], 64 * i + 32, 195, 40, 40);

		if (command_no == i)
		{
			if (state == 0)
			{
				BdispH_AreaFill(64 * i + 2, 64 * i + 59, 172, 172, COLOR_BLACK);
				BdispH_AreaFill(64 * i + 2, 64 * i + 59, 214, 215, COLOR_BLACK);
				BdispH_AreaFill(64 * i + 0, 64 * i + 0, 174, 213, COLOR_BLACK);
				BdispH_AreaFill(64 * i + 60, 64 * i + 61, 174, 213, COLOR_BLACK);
			}
			else if (state == 1)
				BdispH_AreaReverse(64 * i + 1, 64 * i + 60, 173, 214);

			int x = 0;
			int y = 0;
			PrintMini(&x, &y, command_text[command_no], 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
			x = 64 * i + 32 - x / 2;
			y = 155;
			PrintMini(&x, &y, command_text[command_no], 0x42, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
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

int screen_battle::add_effect(int type, int x, int y)
{
	for (int i = 0; i < 10; i++)
	{
		if (!effects[i][0])
		{
			effects[i][0] = type;
			effects[i][1] = x;
			effects[i][2] = y;
			effects[i][3] = 30;
			effects[i][4] = 30;
			return i;
		}
	}
	return -1;
}

bool screen_battle::targetable(int target, int skill_no)
{
	if (target < 1 || target >= 10) return false;
	if (character::bchs[target].mhp <= 0) return false;
	if (skill_no == 1)
		return target == character::bchs[0].next;
	return true;
}

void screen_battle::find_target(int skill_no)
{
	target_timer = 0;
	if (target_no < 1 || target_no > 9) target_no = 1;
	while (!targetable(target_no, skill_no))
	{
		target_no = character::bchs[target_no].next;
		if (target_no > 9) target_no = character::bchs[0].next;
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
	character::bchs[target].wait_gauge = 0;
	if (character::bchs[target].prev != -1)
		character::bchs[character::bchs[target].prev].next = character::bchs[target].next;
	if (character::bchs[target].next != -1)
		character::bchs[character::bchs[target].next].prev = character::bchs[target].prev;
}

void screen_battle::enemy_attack(int attacker)
{
	int x1 = character::bchs[attacker].x;
	int y1 = character::bchs[attacker].y;
	int nx = character::bchs[0].x;
	int ny = character::bchs[0].y - (character::bchs[attacker].height + character::bchs[0].height) / 2;
	int px = x1;
	int py = y1;

	for (int i = 0; i < 6; i++)
		gc.update();

	for (int a = 0; a < 1; a++)
	{
		// 0=Initial -=failed +=success
		int action_command = 0;

		int range = 10 - a * 2;

		for (int f = 0; f < 40 + range / 2; f++)
		{
			if (f <= 40)
			{
				character::bchs[attacker].x = (px * (40 - f) + nx * f) / 40;
				character::bchs[attacker].y = (py * (40 - f) + ny * f + (f - 20) * (f - 20) * 6 - 2400) / 40;
				if (f == 13)
					character::bchs[attacker].rotation = 3;
				else if (f == 26)
					character::bchs[attacker].rotation = 2;
			}
			else if (action_command != 0) break;

			if ((keys.action || keys.f_key[command_no]) && action_command >= 0)
			{
				if (f < 40 - (range + 1) / 2)
					action_command = (f < 20 ? 0 : -1);
				else action_command = 1 + (f >= 40) + (f == 40);
			}
			gc.update();
		}
		damage(0, character::bchs[attacker].attack - (action_command > 0), action_command > 0 ? -3 - action_command : 0);
		if (action_command == 3)
		{
			character::bchs[attacker].rotation = 0;
			int ep = damage(attacker, (character::bchs[0].attack + character::bchs[0].defense) / 2, 0);
			for (int i = 0; i < 20; i++)
			{
				if (i < 5)
				{
					character::bchs[attacker].x += 8 - i;
					character::bchs[attacker].y--;
					effects[ep][1] += 8 - i;
				}
				gc.update();
			}
			break;
		}
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
