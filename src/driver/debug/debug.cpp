#include <driver/debug/debug.hpp>
#include <arch/noarch/debug/debug.hpp>

void kprint(const char *str) {
	debug(str);
}
void kputc(char c) {
	debug_putc(c);
}
