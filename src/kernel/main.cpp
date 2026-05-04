#include <kernel/kernel.hpp>
#include <driver/fb/kprint.hpp>
#include <driver/fb/primitives/line.hpp>
#include <kernel/graphics/color.hpp>

extern "C" void kmain()
{
	kprint("Hello, World! This is a test of the kernel's main function.");
	kprint_at("This text is printed at column 10, row 5.", 10, 5);
	line(10, 10, 100, 100, COLOR_RED);
}
