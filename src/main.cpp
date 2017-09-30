#include "game_control.h"

//PrintCXY(0, 96, "Around the Castle", TEXT_MODE_TRANSPARENT_BACKGROUND, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
//PrintCXY(0, 120, "2017.7.23-2017.8.8", TEXT_MODE_TRANSPARENT_BACKGROUND, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);
//PrintCXY(48, 144, "Celicath", TEXT_MODE_TRANSPARENT_BACKGROUND, -1, COLOR_BLACK, COLOR_WHITE, 1, 0);


#if TARGET_WINSIM
int simmain() {
#else
int main() {
#endif
	gc.start();
	return 0;
}
