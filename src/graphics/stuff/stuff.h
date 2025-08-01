#pragma once

#include "../graphics.h"
#include <devices/debug/debugcon.h>
#include <stdint.h>

void circle(float x, float y, float radius, int color, const VbeModeInfo* vbe_data);

extern "C" void thing(uint8_t x, uint8_t y);