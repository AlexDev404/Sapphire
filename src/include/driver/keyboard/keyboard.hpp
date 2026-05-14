#pragma once
#include <stdint.h>

// PS/2 Keyboard I/O ports
#define KB_DATA_PORT    0x60  // Read scancode from here
#define KB_STATUS_PORT  0x64  // Check status before reading

// Key state flags
#define KB_RELEASED     0x80  // Bit 7 set = key released (break code)

// Ring buffer size (must be power of 2 for easy wrapping)
#define KB_BUFFER_SIZE  256

// Scancode Set 1 → ASCII lookup table (lowercase only for now)
// Index = scancode, value = ASCII character (0 = no printable character)
extern const char scancode_to_ascii[128];

struct Keyboard
{
	// Ring buffer for storing typed characters
	char buffer[KB_BUFFER_SIZE];
	uint32_t read_pos;	// where the consumer reads from
	uint32_t write_pos; // where the ISR writes to

	// Modifier key state
	bool shift_held;
	bool ctrl_held;

	void init();

	// Called by the IRQ1 handler — reads scancode, translates, buffers
	void handle_interrupt();

	// Read one character from the buffer (returns 0 if empty)
	char read_char();

	// Check if there's input available
	bool has_input();
};

namespace keyboard
{
	// Initialize the keyboard driver (registers IRQ1 handler)
	void init_keyboard();

	// Read a character (blocking or non-blocking depending on implementation)
	char read_char();
	// Check if the buffer has anything for us
	bool has_input();
}