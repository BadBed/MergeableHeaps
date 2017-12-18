#include "pch.h"
#include <cstdlib>

int random(int max) {
	if (max <= 0) return 0;
	return (rand() + rand()*RAND_MAX) % (max + 1);
}
int random(int min, int max) {
	return random(max - min) + min;
}