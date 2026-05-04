#include <driver/debug/debug.hpp>
#include <driver/fb/primitives/line.hpp>
#include <kernel/graphics/color.hpp>

void kmain()
{
	kprint("Hello, World! This is a test of the kernel's main function.");
	line(10, 10, 100, 100, COLOR_RED);
}