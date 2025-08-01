#pragma once
#include "vbe_mode_info.h"
#include "fonts/ibm_vga8x16.h"
#include "print_text.hpp"
#include "graphics.hpp"

void drawchar(char chr, int x, int y, unsigned long fgcolor, unsigned long bgcolor);
void drawchar(char chr, int x, int y, unsigned long fgcolor, unsigned long bgcolor, const VbeModeInfo* vbe_data);