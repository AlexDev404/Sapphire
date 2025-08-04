/*
 * Simple PS/2 Keyboard Driver for Sapphire OS
 * Handles basic keyboard input and scan code translation
 */

#include "keyboard.hpp"

volatile uint32_t SHIFT_OFFSET = 0; // Track shift key state
// Basic scan code to ASCII mapping for US QWERTY layout
// This is a simplified mapping for demonstration purposes
static const char scancode_to_ascii[180] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8',	  /* 9 */
	'9', '0', '-', '=', '\b',						  /* Backspace */
	'\t',											  /* Tab */
	'q', 'w', 'e', 'r',								  /* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	  /* Enter key */
	0,												  /* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
	'\'', '`', 0,									  /* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',				  /* 49 */
	'm', ',', '.', '/', 0,							  /* Right shift */
	'*',
	0,	 /* Alt */
	' ', /* Space bar */
	0,	 /* Caps lock */
	0,	 /* 59 - F1 key ... > */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, /* < ... F10 */
	0, /* 69 - Num lock*/
	0, /* Scroll Lock */
	0, /* Home key */
	0, /* Up Arrow */
	0, /* Page Up */
	'-',
	0, /* Left Arrow */
	0,
	0, /* Right Arrow */
	'+',
	0, /* 79 - End key*/
	0, /* Down Arrow */
	0, /* Page Down */
	0, /* Insert Key */
	0, /* Delete Key */
	0, 0, 0,
	0, /* F11 Key */
	0, /* F12 Key */
	0, /* All other keys are undefined */

	// Keyboard set 2 (Post-shift key)

	0, 27, '!', '@', '#', '$', '%', '^', '&', '*',	  /* 9 */
	'(', ')', '_', '+', '\b',						  /* Backspace */
	'\t',											  /* Tab */
	'Q', 'W', 'E', 'R',								  /* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	  /* Enter key */
	0,												  /* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
	'"', '~', 0,									  /* Left shift */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N',				  /* 49 */
	'M', '<', '>', '?', 0,							  /* Right shift */
	'*',
	0,	 /* Alt */
	' ', /* Space bar */
	0,	 /* Caps lock */
	0,	 /* 59 - F1 key ... > */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, /* < ... F10 */
	0, /* 69 - Num lock*/
	0, /* Scroll Lock */
	0, /* Home key */
	0, /* Up Arrow */
	0, /* Page Up */
	'-',
	0, /* Left Arrow */
	0,
	0, /* Right Arrow */
	'+',
	0, /* 79 - End key*/
	0, /* Down Arrow */
	0, /* Page Down */
	0, /* Insert Key */
	0, /* Delete Key */
	0, 0, 0,
	0, /* F11 Key */
	0, /* F12 Key */
	0, /* All other keys are undefined */
};

/**
 * Read scan code from keyboard data port
 * This must be called from the keyboard interrupt handler
 * to clear the interrupt condition
 */
uint8_t keyboard_read_scan_code(void)
{
	return inportb(KEYBOARD_DATA_PORT);
}

/**
 * Convert scan code to ASCII character
 * Returns 0 for non-printable keys
 */
char scancode_to_char(uint8_t scancode)
{
	// Handle key release (high bit set)
	if (scancode & 0x80)
	{
		// Check if it's a shift key release
		if ((scancode & 0x7F) == 0x2A || (scancode & 0x7F) == 0x36)
		{
			SHIFT_OFFSET = 0; // Shift key released
		}
		return 0; // Key release - don't print anything
	}

	// Make sure scancode is within our table bounds
	if (scancode >= 90)
	{
		return 0;
	}

	// Check if shift is pressed
	if (scancode == 0x2A || scancode == 0x36)
	{						// Left or Right Shift
		SHIFT_OFFSET = 90; // Shift key pressed
		return 0;			// Don't print anything for shift key itself
	}

	// Return appropriate character based on shift state
	return scancode_to_ascii[scancode + SHIFT_OFFSET]; // Use shifted characters
}

/**
 * Handle a keyboard scan code
 * This should be called from your keyboard interrupt handler
 */
void keyboard_handle_scancode(uint8_t scancode)
{
	char ascii_char = scancode_to_char(scancode);

	if (ascii_char != 0)
	{
		// Handle printable character
		// You can add your own logic here to:
		// - Add to keyboard buffer
		// - Echo to screen
		// - Process special keys (Enter, Backspace, etc.)

		// Example: Just echo the character for now
		// You'll need to implement or include these functions:
		// putchar(ascii_char);  // or your equivalent function
	}

	// Handle special keys that don't have ASCII equivalents
	switch (scancode)
	{
	case 0x1C: // Enter key
		// Handle Enter
		break;
	case 0x0E: // Backspace
		// Handle Backspace
		break;
	case 0x39: // Space bar
		// Handle Space (already in ASCII table as ' ')
		break;
		// Add more special key handling as needed
	}
}
