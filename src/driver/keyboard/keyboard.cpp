#include <driver/keyboard/keyboard.hpp>
#include <arch/x86/idt/idt.hpp>
#include <arch/x86/pic/pic.hpp>
#include <driver/fb/kprint.hpp>

// Global keyboard instance
static Keyboard kb;

// Scancode Set 1 → ASCII lookup table
// Index = scancode, value = ASCII char.
// 0 means "not a printable character" (shift, ctrl, etc.)
const char scancode_to_ascii[128] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Shifted version of the table (uppercase + symbols)
const char scancode_to_ascii_shift[128] = {
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Scancodes for modifier keys (Scancode Set 1)
#define SC_LSHIFT_PRESS 0x2A
#define SC_RSHIFT_PRESS 0x36
#define SC_LSHIFT_RELEASE 0xAA
#define SC_RSHIFT_RELEASE 0xB6
#define SC_CTRL_PRESS 0x1D
#define SC_CTRL_RELEASE 0x9D

void Keyboard::init()
{
	read_pos = 0;
	write_pos = 0;
	shift_held = false;
	ctrl_held = false;
}

void Keyboard::handle_interrupt()
{
	// Read the scancode from the keyboard data port.
	uint8_t scancode = inb(KB_DATA_PORT);

	// Handle modifier keys (shift, ctrl).
	// Check if the scancode is a shift press/release or ctrl press/release.
	// Update shift_held / ctrl_held accordingly.
	// If it's a modifier key, return early (don't put it in the buffer).
	if (scancode == SC_LSHIFT_PRESS || scancode == SC_RSHIFT_PRESS)
	{
		shift_held = true;
		return;
	}
	if (scancode == SC_LSHIFT_RELEASE || scancode == SC_RSHIFT_RELEASE)
	{
		shift_held = false;
		return;
	}
	if (scancode == SC_CTRL_PRESS)
	{
		ctrl_held = true;
		return;
	}
	if (scancode == SC_CTRL_RELEASE)
	{
		ctrl_held = false;
		return;
	}

	// Ignore key releases (bit 7 set = break code)
	if (scancode & KB_RELEASED)
		return;

	// Translate scancode to ASCII using the lookup table.
	// Use the shifted table if shift is held.

	char c = shift_held ? scancode_to_ascii_shift[scancode] : scancode_to_ascii[scancode];

	// If c is a printable character (c != 0), add it to the ring buffer.
	if (c != 0)
	{
		buffer[write_pos % KB_BUFFER_SIZE] = c;
		write_pos++;
	}
}

char Keyboard::read_char()
{
	// If buffer is empty (read_pos == write_pos), return 0.
	// Otherwise, read one character and advance read_pos.
	if (read_pos == write_pos)
		return 0;
	char c = buffer[read_pos % KB_BUFFER_SIZE];
	read_pos++;
	return c;
}

bool Keyboard::has_input()
{
	return read_pos != write_pos; // replace this
}

// IRQ1 handler — called by the IDT dispatch system
static void keyboard_irq_handler(InterruptFrame *frame)
{
	(void)frame;
	kb.handle_interrupt();
}

void init_keyboard()
{
	kb.init();
	// Register our handler for IRQ1 (interrupt 33)
	register_interrupt_handler(33, keyboard_irq_handler);
}

char kb_read_char()
{
	return kb.read_char();
}

bool kb_has_input()
{
	return kb.has_input();
}
