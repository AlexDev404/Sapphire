#pragma once
#include "vbe_mode_info.h"
#include "fonts/ibm_vga8x16.h"
#include "print_text.h"
#include "graphics.h"

void drawchar(char chr, int x, int y, unsigned long fgcolor, unsigned long bgcolor);