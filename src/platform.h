#pragma once

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "fxcg\display.h"
#include "fxcg\keyboard.h"
#include "fxcg\file.h"
#include "fxcg\registers.h"
#include "fxcg\rtc.h"
#include "fxcg\system.h"
#include "fxcg\serial.h"

#if TARGET_WINSIM
#include <windows.h>
#define LITTLE_E
#else
#define BIG_E
#define override
#define nullptr NULL
#endif

void DmaWaitNext(void);
void DoDMAlcdNonblockStrip(unsigned y1, unsigned y2);

#if !TARGET_WINSIM
inline void* operator new(size_t size){
	return malloc(size);
}
inline void operator delete(void* addr) {
	free(addr);
}
inline void* operator new[](size_t size) {
	return malloc(size);
}
inline void operator delete[](void* addr) {
	free(addr);
}
#endif