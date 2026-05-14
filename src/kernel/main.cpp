#include <kernel/kernel.hpp>
#include <driver/fb/kprint.hpp>
#include <driver/fb/primitives/line.hpp>
#include <driver/keyboard/keyboard.hpp>
#include <kernel/graphics/color.hpp>

extern "C" void kmain()
{
	kprintf("\n\nHello, World! This is a test of the kernel's main function.");
	line(10, 10, 100, 100, COLOR_RED);
	keyboard::init_keyboard();
	kprintf("\n[keyboard]: Keyboard driver is ready\n");
	while (true)
	{
		if (keyboard::has_input())
		{
			char c = keyboard::read_char();
			kprintf("%c", c);
		}
	}
}
