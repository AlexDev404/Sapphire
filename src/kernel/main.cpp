#include <kernel/kernel.hpp>
#include <kernel/syscall.hpp>
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

	// Test the "write" syscall
	const char *buf_wa = "TEST";
	WriteArgs wa = {
		.count = 4,
		.fd = 0,
		.buf = (void *)buf_wa};
	uint32_t res = syscall(SYS_WRITE, (uint32_t)&wa);
	kprintf(" <-- %d characters written from syscall.\n", res);

	// Test the "read" syscall
	char buf_ra[5] = { 0 };
	uint32_t wanted = 4;
	uint32_t total = 0;
	ReadArgs ra;
	
	// Set fd
	ra.fd = 1;
	
	while (total < wanted)
	{
		ra.buf = buf_ra + total;
		ra.count = wanted - total;
		total += syscall(SYS_READ, (uint32_t)&ra);
	}
	kprintf("\n%d characters read from syscall.\n", total);
	kprintf("%s --> read from syscall", buf_ra);
}