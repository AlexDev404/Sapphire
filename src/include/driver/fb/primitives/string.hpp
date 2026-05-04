#pragma once
#include <driver/fb/primitives/char.hpp>

// Print a null-terminated string to the screen
void draw_string(const char *text, unsigned long fgcolor, unsigned long bgcolor, int start_x, int y);

void draw_string_at(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int col,
	int row);
