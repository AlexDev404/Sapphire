#pragma once
#include <arch/noarch/fb/fb.h>
#include "constants/color.hpp"

// Draw a single pixel to the framebuffer
void putpixel(int x, int y, unsigned long color);