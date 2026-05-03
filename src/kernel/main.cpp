#include <arch/noarch/debug/debug.hpp>
#include <kernel/graphics/primitives/line.hpp>
#include <arch/noarch/fb/constants/color.hpp>

extern "C" void kmain()
{
	debug("Hello, World! This is a test of the kernel's main function.");
	line(10, 10, 100, 100, COLOR_RED);
}