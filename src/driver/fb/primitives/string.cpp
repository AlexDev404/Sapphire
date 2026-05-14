#include <driver/fb/primitives/string.hpp>

void draw_string(const char *text, unsigned long fgcolor, unsigned long bgcolor, int start_x, int y)
{
	if (text == nullptr || *text == '\0')
	{
		// If text is null or empty, do nothing
		return;
	}

	int pos = start_x;
	int pos_y = y;
	for (; *text != 0; text++)
	{
		unsigned char b = *text;

		// Handle special characters
		if (b == '\n')
		{
			// Handle newline - move to start of next line
			pos_y += 16;   // Move down by 16 pixels (height of character)
			pos = start_x; // Reset to start of line
			continue;
		}
		else if (b == '\r')
		{
			// Carriage return - move to start of current line
			pos = start_x;
			continue;
		}
		else if (b == '\b')
		{
			// Clear the last character and continue
			// Don't let the cursor be less than zero
			pos = pos <= 0 ? 0 : pos - 9;
			draw_char(' ', pos, pos_y, fgcolor, bgcolor);
			continue;
		}
		else if (b == '\t')
		{
			// Tab - move forward by 4 spaces
			pos += 9 * 4;
			continue;
		}
		else if (b == '\0')
		{
			// Null character - stop processing
			break;
		}
		// Only print printable Extended ASCII characters
		else if (b >= 32 && b <= 254)
		{
			char c = (char)b;
			draw_char(c, pos, pos_y, fgcolor, bgcolor);
		}
		else
		{
			// For other non-printable characters, print a question mark
			draw_char('?', pos, pos_y, fgcolor, bgcolor);
		}

		pos += 9; // Move to next position
	}
}

void draw_string_at(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int col,
	int row)
{
	// Convert character-based coordinates to pixel coordinates
	// Each character is 8x16 pixels with 1 pixel spacing
	int x = col * 9;  // 8 pixels wide + 1 pixel spacing
	int y = row * 16; // 16 pixels tall

	draw_string(text, fgcolor, bgcolor, x, y);
}
