#pragma once
#include <arch/noarch/fb/fb.h>
#include <kernel/graphics/color.hpp>

// Draw a single pixel to the framebuffer
void putpixel(int x, int y, uint32_t color);