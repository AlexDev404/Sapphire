#pragma once

#include "../graphics.h"
#include <devices/debug/debugcon.h>
#include <stdint.h>

void circle(float x, float y, float radius, int color);

extern "C" void thing(unsigned long x, unsigned long y);