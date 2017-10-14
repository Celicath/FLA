#include "fxcg/display.h"

void CopySprite(const color_t* sprite, int x, int y, int width, int height, int mode = 0, int flip = false);
void CopySpriteScale(const color_t* sprite, int x, int y, int width, int height, int mode, int hratio, int vratio, int rotation);

extern const color_t sprite_enemy1[];
extern const color_t sprite_flipp[7][1024];
extern const color_t sprite_command[6][1600];
