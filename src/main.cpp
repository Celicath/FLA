#include "game_control.h"

// started on 2017.7.23 by Celicath

#if TARGET_WINSIM
int simmain() {
#else
int main() {
#endif
	gc.start();
	return 0;
}
