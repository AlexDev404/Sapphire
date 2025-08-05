/*
 * Text printing functions for the Sapphire OS
 * Converted from Rust to C++
 */

#include "vbe_mode_info.h"
#include "drawchar.hpp"

void print_text_logic(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int start_x,
	int y,
	const VbeModeInfo *vbe_mode_info);

// Print a null-terminated string to the screen
void print_text(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int start_x,
	int y)
{
	print_text_logic(text, fgcolor, bgcolor, start_x, y, nullptr);
}
// Print a null-terminated string to the screen
void print_text(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int start_x,
	int y,
	const VbeModeInfo *vbe_mode_info)
{
	print_text_logic(text, fgcolor, bgcolor, start_x, y, vbe_mode_info);
}

void print_text_logic(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int start_x,
	int y,
	const VbeModeInfo *vbe_mode_info)
{
	if (vbe_mode_info == nullptr)
	{
		// If no VBE mode info is provided, use default values
		vbe_mode_info = vbe_block();
	}

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
			drawchar(c, pos, pos_y, fgcolor, bgcolor, vbe_mode_info);
		}
		else
		{
			// For other non-printable characters, print a question mark
			drawchar('?', pos, pos_y, fgcolor, bgcolor, vbe_mode_info);
		}

		pos += 9; // Move to next position
	}
}

// Function to print at specific row/column using character-based coordinates
void print_at(
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

	print_text(text, fgcolor, bgcolor, x, y);
}

// Helper function to convert a 16-bit integer to a string
int u16_to_string(unsigned short value, unsigned char *buffer, int buffer_size)
{
	if (buffer_size < 6)
	{ // Max 5 digits + null terminator
		return 0;
	}

	if (value == 0)
	{
		buffer[0] = '0';
		buffer[1] = 0;
		return 2;
	}

	unsigned short val = value;
	int i = 0;

	while (val > 0 && i < buffer_size - 1)
	{
		buffer[i] = '0' + (val % 10);
		val /= 10;
		i++;
	}

	// Reverse the digits in-place
	int start = 0;
	int end = i - 1;
	while (start < end)
	{
		unsigned char temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
		start++;
		end--;
	}

	// Add null terminator
	buffer[i] = 0;

	return i + 1; // Return length including null terminator
}

// Function to print a 16-bit integer
void print_u16(
	unsigned short value,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int x,
	int y)
{
	unsigned char buffer[6]; // Max 5 digits + null terminator
	u16_to_string(value, buffer, 6);
	print_text(reinterpret_cast<const char *>(buffer), fgcolor, bgcolor, x, y);
}