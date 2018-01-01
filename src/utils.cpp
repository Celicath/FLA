#include "utils.h"

float Q_rsqrt(float f)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = f * 0.5f;
	y = f;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));
	return y;
}

unsigned int RNG;

void sran(unsigned int k)
{
	RNG = k;
}

unsigned int ran()
{
	RNG = RNG * 184110937 + 267143;

	return (RNG >> 2) + (RNG << 30);
}