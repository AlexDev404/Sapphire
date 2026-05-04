#include <driver/fb/primitives/char.hpp>
#include <driver/fb/fb.hpp>

// Character drawing function implementation
void draw_char(char chr, int x, int y, unsigned long fgcolor, unsigned long bgcolor) {
	// Get character index in the font table
	unsigned char *char_data = (unsigned char *)&IBM_VGA_8X16[(unsigned char)chr * 16];

	// Character dimensions
	const int CHAR_WIDTH = 8;
	const int CHAR_HEIGHT = 16;

	// Draw each row of the character
	for (int cy = 0; cy < CHAR_HEIGHT; cy++)
	{
		// Get the current row of pixel data
		unsigned char row = char_data[cy];

		// Draw each column of the character
		for (int cx = 0; cx < CHAR_WIDTH; cx++)
		{
			// Determine if this pixel should be foreground or background
			unsigned long color = (row & (1 << (CHAR_WIDTH - 1 - cx))) ? fgcolor : bgcolor;

			// Use the existing putpixel function to draw the pixel
			putpixel(x + cx, y + cy, color);
		}
	}
}