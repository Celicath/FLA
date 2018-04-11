#include "fxcg/display.h"

// draw sprite (with color mode and flip)
void CopySprite(const color_t* sprite, int x, int y, int width, int height, int mode = 0, int flip = false);

/* draw sprite with specified ratio and rotation
 * scales by (hratio/16, vratio/32)
 * rotates by (rotation*90) degrees */
void CopySpriteScale(const color_t* sprite, int x, int y, int width, int height, int mode, int hratio, int vratio, int rotation);

// draw sprite with alpha (32=opaque, 0=transparent)
void CopySpriteAlpha(const color_t* sprite, int x, int y, int width, int height, int mode, int alpha);

extern const color_t sprite_enemy1[];
extern const color_t sprite_flipp[7][1024];
extern const color_t sprite_command[3][1024];
extern const color_t sprite_icons[9][1024];