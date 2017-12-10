#include "pch.h"
#include <cstdlib>

int random(int max) {
	return (rand() + rand()*RAND_MAX) % (max + 1);
}
int random(int min, int max) {
	return random(max - min) + min;
}